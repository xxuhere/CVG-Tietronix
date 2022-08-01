#include "ManagedCam.h"
#include <opencv2/imgcodecs.hpp>
#include "../Utils/multiplatform.h"
#include "../Utils/cvgStopwatch.h"
#include "../Utils/cvgStopwatchLeft.h"

#include "CamImpl/CamImpl_OCV_USB.h"
#include "CamImpl/CamImpl_OCV_Web.h"
#include "CamImpl/CamImpl_OCV_HWPath.h"
#include "CamImpl/CamImpl_StaticImg.h"
#include <iostream>
#include "../Utils/cvgAssert.h"
#include "../Utils/yen_threshold.h"

#if !_WIN32
	#include "CamImpl/CamImpl_MMAL.h"
#endif

ManagedCam::ManagedCam(VideoPollType pt, int cameraId, const cvgCamFeedSource& camOptions)
{
	this->cameraId		= cameraId;
	this->pollType		= pt;
	this->camOptions = camOptions;
}

ManagedCam::~ManagedCam()
{
	this->CloseVideo();

	// Shut down the thread just in case it's still open, although
	// it's recommended to shut them down with the CamStreamMgr
	// owner, so all ManagedCams can be shut down in parallel.

	if(this->camStreamThread != nullptr)
	{
		this->ShutdownThread();

		while(!this->_isShutdown)
			MSSleep(10);

		this->_JoinThread();
	}
}

void ManagedCam::ThreadFn(int camIdx)
{
	this->_isStreamActive = false;

	// This will be the loop for the thread for the lifetime of the app
	// once booted. This should NOT be confused with the polling loop
	// of the camera, which will be an inner loop.
	while(this->_sentShutdown == false)
	{
		// Local copy in case a thread somehow changes this->pollType;
		VideoPollType pollTy = this->pollType;

		// Check if we're in here because the polling type changed
		if(this->currentImpl != nullptr)
		{
			if(this->currentImpl->PollType() != pollTy)
				this->_ClearImplementation();
		}

		if(pollTy != VideoPollType::Deactivated)
		{ 
			this->conState = State::Connecting;
			this->SwitchImplementation(pollTy);
		}
		else
			this->conState = State::Idling;

		// If we have a camera implementation, start it up.
		if(this->currentImpl != nullptr && this->currentImpl->IsValid())
		{
			this->streamWidth = -1;
			this->streamHeight = -1;

			cvgStopwatch swFPS;
			cvgStopwatchLeft swLoopSleep;

			this->streamFrameCt = 0;

			while( // Polling loop
				this->currentImpl->PollType() == pollTy &&
				this->currentImpl->IsValid() && 
				this->_sentShutdown == false)
			{
				this->conState = State::Polling;

				// Poll the current frame from OpenCV.
				cv::Ptr<cv::Mat> frame = this->currentImpl->PollFrame();

				if(frame != nullptr && !frame.empty())
				{
					// First frame we take the dimension and assume the video size is constant.
					// We could also put in a mechanism (into ICamImpl) to give us a frame size
					// right after activation, but that currently doesn't exist.
					if(this->streamWidth == -1)
					{
						this->streamWidth = frame->cols;
						this->streamHeight = frame->rows;
					}

					// Pass it through to the image pipeline, and then 
					// transfer it to the last frame cache to stage it for
					// other threads (the main GUI thread) to access.
					_FinalizeHandlingPolledImage(frame);

					++this->streamFrameCt;
					this->msInterval = swFPS.Milliseconds();
					int msLeft = swLoopSleep.MSLeft33();
					MSSleep(msLeft);
				}
				else
				{
					// Some implementations block, others don't. For the non-blocking stream
					// we have to make sure not to count them - and we'll give them some breathing
					// room before we poll for a frame again.
					MSSleep(10);
				}
			}
			this->currentImpl->Deactivate();
			this->_DeactivateStreamState();
		}

		// Are we just idling?
		if(pollTy != VideoPollType::Deactivated)
		{
			// If no active polling method is selected, we 
			// wait a bit and recheck the polling method again 
			// in a short while.
			//
			// The wait time is a balance between lowering computational
			// overhead for just idling, and providing the user with
			// an immediate response once a change happens.
			this->conState = State::Idling;
			MSSleep(500);
		}
	}

	this->_ClearImplementation();
	this->_EndShutdown();
	this->conState = State::Shutdown;
}


bool ManagedCam::SwitchImplementation(VideoPollType newImplType, bool delCurrent)
{
	//
	//		REDUNDANCY AVOIDANCE
	//////////////////////////////////////////////////
	// If the same implementation, do nothing.
	if(this->currentImpl != nullptr)
	{
		if(this->currentImpl->PollType() == newImplType)
			return false;

		this->_ClearImplementation();
	}
	else if(newImplType == VideoPollType::Deactivated)
		return false;

	//
	//		SELECTING IMPLEMENTATION
	//////////////////////////////////////////////////
	// Anything created will use a default constructor, actual
	// options and destinations are pulled later with the call
	// to currentImpl->PullOptions() below.
	//
	switch(newImplType)
	{
	default:
		cvgAssert(false,"Unhandled implementation switch");
	case VideoPollType::Deactivated:
		// Does nothing, and currentImpl should be left nullptr.
		std::cout << "Deactivated " << std::endl;
		return true;

	case VideoPollType::OpenCVUSB_Idx:
		std::cout << "OpenCVUSB_Idx " << std::endl;
		this->currentImpl = new CamImpl_OCV_USB(0);
		break;

	case VideoPollType::OpenCVUSB_Named:
		std::cout << "OpenCVUSB_Named " << std::endl;
		this->currentImpl = new CamImpl_OCV_HWPath("");
		break;

	case VideoPollType::Web:
		std::cout << "Web " << std::endl;
		this->currentImpl = new CamImpl_OCV_Web("");
		break;

	case VideoPollType::Image:
		std::cout << "Image " << std::endl;
		this->currentImpl = new CamImpl_StaticImg("");
		break;

#if IS_RPI
	case VideoPollType::MMAL:
		this->currentImpl = new CamImpl_MMAL(0);
		break;
#endif
	}

	//
	//		BOOT UP IMPLEMENTATION
	//////////////////////////////////////////////////
	// Before the implementation is usable, we need to initialize it.
	// While Initialize(), in theory, only needs to be done once, we create
	// these implementation on the fly instead of caching them for the entire
	// lifetime, so these brand new implementation will need an Initialize().
	cvgAssert(this->currentImpl != nullptr,"currentImpl is null");
	//
	// Let the implementation library initialize any data it needs to.
	if(!this->currentImpl->Initialize())
	{
		delete this->currentImpl;
		this->currentImpl = nullptr;
	}

	this->currentImpl->PullOptions(this->camOptions);

	// Actually turn on the device for use.
	if(!this->currentImpl->Activate())
	{
		this->currentImpl->Shutdown();
		delete this->currentImpl;
		this->currentImpl = nullptr;
	}

	this->pollType = newImplType;

	return true;
}

cv::Ptr<cv::Mat> ManagedCam::ImgProc_Simple(cv::Ptr<cv::Mat> src, double threshold)
{
	cv::Ptr<cv::Mat> grey;
	if (src->elemSize() != 1)
	{
		grey = new cv::Mat();
		cv::cvtColor(*src, *grey, cv::COLOR_RGBA2GRAY, 0);
	}
	else 
		grey = src;

	cv::threshold(
		*grey,
		*grey,
		threshold,
		255,
		cv::THRESH_BINARY);

	return grey;
}


cv::Ptr<cv::Mat> ManagedCam::ImgProc_YenThreshold(cv::Ptr<cv::Mat> src, bool compressed)
{
	//Histogram constants
	// Quantize the hue to 30 levels
	// and the saturation to 32 levels
	int bins = 256;
	int histSize[] = { bins };
	// hue varies from 0 to 179, see cvtColor
	const float range[] = { 0, 256 };
	const float* ranges[] = { range };
	cv::Mat hist;
	// we compute the histogram from the 0-th channel
	int channels[] = { 0 };

	/// Convert it to gray (if it's not already).
	cv::Ptr<cv::Mat> grey;
	if(src->elemSize() != 1)
	{
		grey = new cv::Mat();
		cv::cvtColor(*src, *grey, cv::COLOR_RGBA2GRAY, 0);
	}
	else
		grey = src;

	//equalize
	cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
	clahe->setClipLimit(2.7);
	cv::Mat cl;
	clahe->apply(*grey, cl);

	//make histogram
	calcHist(&cl, 1, channels, cv::Mat(), // do not use mask
		hist, 1, histSize, ranges,
		true, // the histogram is uniform
		false);

	// yen_thresholding
	int yen_threshold = Yen(hist);
	cv::Mat thresholded; 
	//std::cout << "Yen threshold : " << yen_threshold << "\n";
	if(compressed)
	{ 
		cv::threshold(cl,
			thresholded,
			double(yen_threshold),
			255,
			cv::THRESH_BINARY);
	}
	else
	{ 
		cv::threshold(cl,
			thresholded,
			double(yen_threshold),
			255,
			cv::THRESH_TOZERO);
	}
	//Note THRESH_TO_ZERO is only one option another, possibly better option is THRESH_BINARY
	 
	if (compressed)
		return new cv::Mat(thresholded);

	//blur
	cv::Mat blurred; 
	cv::medianBlur(thresholded, blurred, 7);

	//Note the next steps are expensive and possibly unnecesaary keeping them for completeness
	//edges
	cv::Mat edges;
	cv::Canny(blurred, edges, 120, 120);

	//dialate
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3), cv::Point(1, 1));
	cv::Mat dialated;
	cv::dilate(edges, dialated, kernel);

	//flood
	cv::Mat flooded = dialated.clone();
	cv::floodFill(flooded, cv::Point(0, 0), cv::Scalar(255));

	//invert
	cv::Mat invert;
	cv::bitwise_not(flooded, invert);
	cv::Mat* inverted = new cv::Mat(invert);
	return inverted;//Note might be worth changing this to blurred after changing thresholding ot use THRESH_BINARY

}

cv::Ptr<cv::Mat> ManagedCam::ImgProc_TwoStDevFromMean(cv::Ptr<cv::Mat> src)
{
	// make sure greyscale first
	cv::Ptr<cv::Mat> grey;
	if (src->elemSize() != 1)
	{
		grey = new cv::Mat();
		cv::cvtColor(*src, *grey, cv::COLOR_RGBA2GRAY, 0);
	}
	else 
		grey = src;

	cv::Mat mean, stddev;
	cv::meanStdDev(*grey, mean, stddev);
	double final_mean = mean.at<double>(0, 0);
	double final_stddev = stddev.at<double>(0, 0);

	return ImgProc_Simple(grey, final_mean + 2.0 * final_stddev);
}

cv::Ptr<cv::Mat> ManagedCam::ProcessImage(cv::Ptr<cv::Mat> inImg)
{
	// When modifying this function, make sure to sync with IsThresholded().
	switch (this->camOptions.processing)
	{
	case ProcessingType::None:
		return inImg;

	case ProcessingType::yen_threshold:
		return ImgProc_YenThreshold(inImg, false);

	case ProcessingType::yen_threshold_compressed:
		return ImgProc_YenThreshold(inImg, true);

	case ProcessingType::two_stdev_from_mean:
		return ImgProc_TwoStDevFromMean(inImg);

	case ProcessingType::static_threshold:
		return ImgProc_Simple(inImg, this->camOptions.thresholdExplicit);
	}

	cvgAssert(false,"Unhandled processing switch");
	return inImg;
}

bool ManagedCam::IsThresholded()
{
	return this->camOptions.processing != ProcessingType::None;
}

bool ManagedCam::UsesImageProcessingChain()
{
	return this->IsThresholded();
}

void ManagedCam::SetPoll(VideoPollType pollTy)
{
	this->pollType = pollTy;
}

ProcessingType ManagedCam::GetProcessingType() const
{
	return this->camOptions.processing;
}

float ManagedCam::GetFloat( StreamParams paramid)
{
	switch(paramid)
	{
	case StreamParams::StaticThreshold:
		return (float)this->camOptions.thresholdExplicit;
		break;
	}

	return this->IManagedCam::GetFloat(paramid);
}

bool ManagedCam::SetFloat( StreamParams paramid, float value)
{
	switch(paramid)
	{
	case StreamParams::StaticThreshold:
		this->camOptions.thresholdExplicit = (int)std::clamp(value, 0.0f, 255.0f);
		return true;
	}

	return this->IManagedCam::SetFloat(paramid, value);
}

bool ManagedCam::SetProcessingType(ProcessingType pt)
{
	this->camOptions.processing = pt;
	return true;
}

void ManagedCam::_ClearImplementation(bool delCurrent, bool resetPollTy)
{
	if(this->currentImpl != nullptr)
	{
		this->currentImpl->Deactivate();

		if(delCurrent)
		{
			this->currentImpl->Shutdown();
			delete this->currentImpl;
		}

		this->currentImpl = nullptr;
	}
	if(resetPollTy)
		this->pollType = VideoPollType::Deactivated;
}

CamType ManagedCam::GetCamType()
{
	return CamType::VideoFeed;
}

int ManagedCam::GetID() const
{
	return this->cameraId;
}

std::string ManagedCam::GetStreamName() const
{
	return std::to_string(this->cameraId);
}

void ManagedCam::_DeactivateStreamState(bool deactivateShould)
{
	if(deactivateShould)
		this->pollType = VideoPollType::Deactivated;

	this->IManagedCam::_DeactivateStreamState(deactivateShould);
}

void ManagedCam::InjectIntoDicom(DcmDataset* dicomData)
{
	if(currentImpl != nullptr && currentImpl->IsValid())
		currentImpl->DelegatedInjectIntoDicom(dicomData);
}