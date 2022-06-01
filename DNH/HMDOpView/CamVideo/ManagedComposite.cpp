#include "ManagedComposite.h"

#include "../Utils/cvgAssert.h"
#include "../Utils/cvgStopwatch.h"
#include "../Utils/cvgStopwatchLeft.h"
#include "../Utils/multiplatform.h"

// Imitating how compositing happens in OpenGL for StateHMDOp
#include "../Utils/cvgRect.h"

#include <mutex>

bool ManagedComposite::cacheAvailable;
std::map<int, cv::Ptr<cv::Mat>> ManagedComposite::globalCache;
std::mutex ManagedComposite::cacheMutex;
bool ManagedComposite::modSinceLastCache = true;

bool ManagedComposite::CacheCameraFrame(int id, cv::Ptr<cv::Mat> img)
{
	std::lock_guard<std::mutex> guard(cacheMutex);
	if(!cacheAvailable)
		return false;

	modSinceLastCache = true;
	globalCache[id] = img;
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
			std::map<int, cv::Ptr<cv::Mat>> cacheCpy;
			{
				std::lock_guard<std::mutex> cpyGuard(cacheMutex);
				std::swap(cacheCpy, globalCache);
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
				auto matCache = it.second;

				float vaspect = (float)matCache->rows / (float)matCache->cols;
				cvgRect rect = cvgRect::MakeWidthAspect(this->streamWidth, vaspect);
				cv::Mat cpy;
				cv::resize(
					*it.second, 
					cpy, 
					cv::Size(
						(int)rect.w, 
						(int)rect.h));

				// Match the "red-ing" used in StateHMDOp
				if(cpy.channels() == 1)
				{
					// https://stackoverflow.com/questions/26065253/opencv-set-a-channel-to-a-value-c
					cpy *= 0.5; // Ret
					std::vector<cv::Mat> chansComp
					{
						cv::Mat(cpy.rows, cpy.cols, CV_8UC1, cv::Scalar(0)),	// B
						cv::Mat(cpy.rows, cpy.cols, CV_8UC1, cv::Scalar(0)),	// G
						cpy // R
					};
					cv::merge(chansComp, cpy);
				}

				// For how OpenCV functions work, images need to be of the same
				// size (and probably the same channel count) - but we may not
				//be guaranteed that 
				// - Each camera/video-feed is the same size
				// - The composite output is the same size as the video feeds.
				// To compensate, we need to figure out a common "region of interest"
				// (ROI) between the composite source and destination (accumFrame).

				// Figure out the region of interest for the blitting.
				// For now we're just centering.
				int roiAx = 0;
				int roiAy = 0;
				int roiAw = this->streamWidth;
				int roiAh = this->streamHeight;

				int roiBx = 0;
				int roiBy = 0;
				int roiBw = cpy.cols;
				int roiBh = cpy.rows;

				if(roiAw > roiBw)
				{
					roiAx = (int)(roiAw - roiBw) * 0.5f;
					roiAw = roiBw;
				}
				else
				{
					roiBx = (int)(roiBw - roiAw) * 0.5f;
					roiBw = roiAw;
				}

				if(roiBh > roiBh)
				{
					roiAy = (int)(roiAh - roiBh) * 0.5f;
					roiAh = roiBh;
				}
				else
				{
					roiBy = (int)(roiBh - roiAh) * 0.5f;
					roiBh = roiAh;
				}

				cv::Mat acRoi = (*accumframe)(cv::Rect(roiAx, roiAy, roiAw, roiAh));
				cv::Mat cpyRoi = cpy(cv::Rect(roiBx, roiBy, roiBw, roiBh));
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

float ManagedComposite::GetFloat( StreamParams paramid)
{
	switch(paramid)
	{
	case StreamParams::CompositeVideoWidth:
		return this->streamWidth;

	case StreamParams::CompositeVideoHeight:
		return this->streamHeight;
	}

	return this->IManagedCam::GetFloat(paramid);
}

bool ManagedComposite::SetFloat( StreamParams paramid, float value)
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

	return this->IManagedCam::SetFloat(paramid, value);
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