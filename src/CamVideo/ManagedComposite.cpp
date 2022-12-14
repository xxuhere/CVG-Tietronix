
// Issue with wxWidgets and DCMTK requiring different
// include orders of different platforms.
#if WIN32
	#include "../Utils/cvgRect.h"
	#include "ManagedComposite.h"
#else
	#include "ManagedComposite.h"
	#include "../Utils/cvgRect.h"
#endif

#include "../Utils/cvgAssert.h"
#include "../Utils/cvgStopwatch.h"
#include "../Utils/cvgStopwatchLeft.h"
#include "../Utils/multiplatform.h"

// Imitating how compositing happens in OpenGL for StateHMDOp
#include "ROIRect.h"

#include <mutex>


bool ManagedComposite::cacheAvailable;
std::map<int, CompCacheInfo> ManagedComposite::globalCache;
std::mutex ManagedComposite::cacheMutex;
bool ManagedComposite::modSinceLastCache = true;

CompCacheInfo::CompCacheInfo()
{}

CompCacheInfo::CompCacheInfo(
	cv::Ptr<cv::Mat> img, 
	bool thresholded, 
	float opacity)
{
	this->img			= img;
	this->thresholded	= thresholded;
	this->opacity		= opacity;
}

bool ManagedComposite::CacheCameraFrame(
	int id, 
	cv::Ptr<cv::Mat> img, 
	bool thresholded, 
	float opacity)
{

	CompCacheInfo cInfo(img, thresholded, opacity);

	std::lock_guard<std::mutex> guard(cacheMutex);
	if(!cacheAvailable)
		return false;

	modSinceLastCache = true;
	globalCache[id] = cInfo;
	return true;
}

cv::Ptr<cv::Mat> ManagedComposite::ProcessImage(cv::Ptr<cv::Mat> inImg)
{
	// No processing performed, just relay it.
	return inImg;
}

bool ManagedComposite::UsesImageProcessingChain()
{
	return false;
}

void ManagedComposite::_EndShutdown()
{
	std::lock_guard<std::mutex> guard(cacheMutex);
	cacheAvailable = false;
	globalCache.clear();
}

void ManagedComposite::ThreadFn(int camIdx)
{
	{
		std::lock_guard<std::mutex> guard(cacheMutex);

		// It might be pedantic, but if they're not using the correct
		// special ID, other things might also be wrong.
		//
		// Plus, GetID() is already hardcoded to return this value.
		cvgAssert(
			camIdx == SpecialCams::Composite, 
			"Attempting to boot thread for misidentified ManagedComposite");

		this->_isStreamActive = false;

		this->streamWidth = 1920;
		this->streamHeight = 1080;

		// It's not really dependent on anything for initialization, so
		// might as well always pretend to be polling to show we're active.
		this->conState = State::Polling;
		cacheAvailable = true;
	}

	cvgStopwatch swFPS;
	cvgStopwatchLeft swLoopSleep;

	this->streamFrameCt = 0;
	{ 
		// Scope so the local variable to initialize the starting
		// nothing-image doesn't live for the entire life of the 
		// threa function.
		cv::Ptr<cv::Mat> initFrame = new cv::Mat(this->streamHeight, this->streamWidth, CV_8UC3);
		this->_FinalizeHandlingPolledImage(initFrame);
	}

	// The compositing loop doesn't poll its own data. Instead, it gets
	// pushed data from external sources.
	while(this->_sentShutdown == false)
	{
		// If anything new, recomposite
		if(this->modSinceLastCache)
		{
			// We make a copy so afterwards, we have free reign on a snapshot of 
			// the globalCache without keeping it locked for as long as we need
			// it for however long compositing takes
			std::map<int, CompCacheInfo> cacheCpy;
			{
				std::lock_guard<std::mutex> cpyGuard(cacheMutex);

				// Get a copy, so if the pointers change, that doesn't affect
				// what we're rendering to. While not necessarily a bad thing,
				// when dealing with thread, our consideration is to make dealing
				// with this data as stable as possible. 
				//
				// This use to swap out the contents instead of copy, but this
				// could lead to a situation where the next pass doesn't re-supply
				// all the composited cameras fast enough - leading to missing
				// compositing elements randomly dropping out in the footage.
				cacheCpy = globalCache;
			}

			// The composite target. For now we'll start with black at the final
			// save dimensions and add aall contributing images on top, similar
			// (as similar as possible) to how StateHMDOp works.
			cv::Ptr<cv::Mat> accumframe = 
				new cv::Mat(
					this->streamHeight, 
					this->streamWidth, 
					CV_8UC3,
					cv::Scalar(0, 0, 0));

			for(auto it : cacheCpy)
			{
				auto matCache = it.second.img;

				float vaspect = (float)matCache->rows / (float)matCache->cols;
				cvgRect rect = cvgRect::MakeWidthAspect(this->streamWidth, vaspect);
				cv::Mat cpy;
				cv::resize(
					*it.second.img, 
					cpy, 
					cv::Size(
						(int)rect.w, 
						(int)rect.h));

				// Single channel image needs to be converted to RGB, need to figure
				// out if we make it thresholded red, or greyscale.
				if(cpy.channels() == 1)
				{
					// https://stackoverflow.com/questions/26065253/opencv-set-a-channel-to-a-value-c
					cpy *= it.second.opacity; // Ret

					std::vector<cv::Mat> chansComp;
					if(it.second.thresholded)
					{
						// Make red
						chansComp.push_back(cv::Mat(cpy.rows, cpy.cols, CV_8UC1, cv::Scalar(0)));	// B
						chansComp.push_back(cv::Mat(cpy.rows, cpy.cols, CV_8UC1, cv::Scalar(0)));	// G
						chansComp.push_back(cpy); // R
					}
					else
					{
						// Make greyscale
						chansComp.push_back(cpy);	// B
						chansComp.push_back(cpy);	// G
						chansComp.push_back(cpy);	// R
					}
					
					cv::merge(chansComp, cpy);
				}

				// For how OpenCV functions work, images need to be of the same
				// size (and probably the same channel count) - but we may not
				//be guaranteed that 
				// - Each camera/video-feed is the same size
				// - The composite output is the same size as the video feeds.
				// To compensate, we need to figure out a common "region of interest"
				// (ROI) between the composite source and destination (accumFrame).
				//
				// Figure out the region of interest for the blitting.
				// For now we're just centering.
				

				// STEP 1 : DECLARE OUR REPRESENTATION OF THE RECTANGLES
				//////////////////////////////////////////////////

				// The rect of the whole destination region.
				ROIRect roiActDst(0, 0, this->streamWidth, this->streamHeight);
				//
				// The rect of the DST where the DST will be placed into.
				ROIRect roiVirtDst(
					(this->streamWidth - cpy.cols)/2,		// Centered
					(this->streamHeight - cpy.rows)/2,		// Centered
					cpy.cols, 
					cpy.rows);
				//
				// ROI for the source image being composited
				// (wleu 07/11/2022)
				ROIRect roiSrc(
					0, 
					0,
					cpy.cols,
					cpy.rows);

				// STEP 2 : CLIP
				//////////////////////////////////////////////////
				// Make sure roiVirtDst is bounded to roiActDst to make sure the
				// image will into the drawable area. Any change to where the
				// image will be drawn to, will also change the position and
				// dimensions of where we take our source pixels from.
				if(roiVirtDst.x < 0)
				{
					// Keep the right of the roiVirtDst rect in place, but move the left
					// over to be at 0.
					roiVirtDst.w -= roiVirtDst.x;	// Add the absolute value, remember it's negative right now
					roiSrc.x -= roiVirtDst.x;		// Apply same to non-virtual
					roiSrc.w = roiVirtDst.w;		// Apply same to non-virtual, they started the same so they'll end the same.
					roiVirtDst.x = 0;			
				}
				if(roiVirtDst.y < 0)
				{
					roiVirtDst.h -= roiVirtDst.y;	// Add the absolute value, remember it's negative right now
					roiSrc.y -= roiVirtDst.y;		// Apply same to non-virtual
					roiSrc.h = roiVirtDst.h;		// Apply same to non-virtual, they started the same so they'll end the same.
					roiVirtDst.y = 0;
				}
				if(roiVirtDst.Right() > roiActDst.Right())
				{
					int overAmt = roiVirtDst.Right() - roiActDst.Right();
					roiVirtDst.w -= overAmt;		// Move the right inwards so it's not out of bounds.
					roiSrc.w -= overAmt;			// Apply same to non-virtual
				}
				if(roiVirtDst.Bottom() > roiActDst.Bottom())
				{
					int overAmt = roiVirtDst.Bottom() - roiActDst.Bottom();
					roiVirtDst.h -= overAmt;		// Move the bottom inwards so it's not out of bounds.
					roiSrc.h -= overAmt;			// Apply same to non-virtual
				}

				// STEP 3 : BLITT
				//////////////////////////////////////////////////


				// Draw the region of the image being composited into the
				// compositing canvas in a way where we identify perfectly
				// what parts will map to what images their pixels are
				// mapped to (roiVirtDst.w/h should match roiDst.w/h and 
				// both should be in bounds of their respective image) while
				// producting an image similar to how things are drawn in
				// the display.
				cv::Mat acRoi = (*accumframe)(roiVirtDst.ToCVRect());
				cv::Mat cpyRoi = cpy(roiSrc.ToCVRect());

				if(cpyRoi.channels() == 4)
				{
					// Add an RGB-A
					// Alpha is a per-pixel gain for each channel.
					// Although chances are the alpha will be binary, so it's 
					// simply a pixel gate.
					std::vector<cv::Mat> mats(4);
					cv::split(cpyRoi, &mats[0]);

					cv::multiply(mats[0], mats[3], mats[0], 1.0/255.0);
					cv::multiply(mats[1], mats[3], mats[1], 1.0/255.0);
					cv::multiply(mats[2], mats[3], mats[2], 1.0/255.0);
					cv::merge(&mats[0], 3, cpyRoi);

				}

				cv::add(acRoi, cpyRoi, acRoi);
			}

			_FinalizeHandlingPolledImage(accumframe);
		}
		else
		{ 
			// Feed it back into itself. We need to make sure nothing modifies
			// the image during the final handler.
			_FinalizeHandlingPolledImage(this->curCamFrame);
		}

		++this->streamFrameCt;
		this->msInterval = swFPS.Milliseconds();
		int msLeft = swLoopSleep.MSLeft33();
		MSSleep(msLeft);
				
	}
	this->_EndShutdown();
	this->conState = State::Shutdown;
}

double ManagedComposite::GetParam( StreamParams paramid)
{
	switch(paramid)
	{
	case StreamParams::CompositeVideoWidth:
		return this->streamWidth;

	case StreamParams::CompositeVideoHeight:
		return this->streamHeight;
	}

	return this->IManagedCam::GetParam(paramid);
}

bool ManagedComposite::SetParam( StreamParams paramid, double value)
{
	
	switch(paramid)
	{
	case StreamParams::CompositeVideoWidth:
		this->streamWidth = (int)value;
		return true;

	case StreamParams::CompositeVideoHeight:
		this->streamHeight = (int)value;
		return true;
	}

	return this->IManagedCam::SetParam(paramid, value);
}

CamType ManagedComposite::GetCamType()
{
	return CamType::SpecialType;
}

int ManagedComposite::GetID() const
{
	return SpecialCams::Composite;
}

std::string ManagedComposite::GetStreamName() const
{
	return "COMP";
}