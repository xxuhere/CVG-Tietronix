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

bool ManagedCam::ShutdownThread()
{
	if(this->camStreamThread == nullptr)
		return true;

	// Send the signal for the thread the shut down. Then it's up to
	// to the thread to see the signal and shut itself down.
	this->_sentShutdown = true;
	return true;
}

bool ManagedCam::_JoinThread()
{
	if(this->camStreamThread == nullptr || !this->_isShutdown)
		return false;

	this->camStreamThread->join();
	delete this->camStreamThread;
	this->camStreamThread = nullptr;
	return true;
}

cv::Ptr<cv::Mat> ManagedCam::GetCurrentFrame()
{
	// Get a copy of the shared ptr in a controlled manner, when we
	// know it's stable.
	cv::Ptr<cv::Mat> cpyRet;

	{ // Mutex guard scope
		std::lock_guard<std::mutex> guard(this->imageAccess);
		cpyRet = this->curCamFrame;
	}
	return cpyRet;
}

bool ManagedCam::SetCurrentFrame(cv::Ptr<cv::Mat> mat)
{
	std::lock_guard<std::mutex> guard(this->imageAccess);
	this->curCamFrame = mat;
	++this->camFeedChanges;
	return true;
}

SnapRequest::SPtr ManagedCam::RequestSnapshot(const std::string& filename)
{
	SnapRequest::SPtr req = SnapRequest::MakeRequest(filename);
	{
		// Thread protected add to the to-process list.
		std::lock_guard<std::mutex> guard(this->snapReqsAccess);
		req->status = SnapRequest::Status::Requested;
		this->snapReqs.push_back(req);
	}
	return req;
}

void ManagedCam::ClearSnapshotRequests()
{
	std::vector<SnapRequest::SPtr> swapDst;
	{
		std::lock_guard<std::mutex> guard(this->snapReqsAccess);
		std::swap(swapDst, this->snapReqs);
	}

	for(SnapRequest::SPtr s : swapDst)
	{
		// Technically not an error, but a cancellation. But, this
		// will do.
		s->status = SnapRequest::Status::Error;
	}
}

VideoRequest::SPtr ManagedCam::OpenVideo(const std::string& filename)
{
	std::lock_guard<std::mutex> guard(this->videoAccess);

	if (filename.empty())
	{
		this->_CloseVideo_NoMutex();
		VideoRequest::SPtr noneRet = VideoRequest::MakeRequest(0, 0, this->cameraId, filename);
		noneRet->err = "Empty filename";
		noneRet->status = VideoRequest::Status::Error;
		return noneRet;
	}

	// If we're already recording the target, just sent back the
	// current active request.
	if(this->activeVideoReq != nullptr)
	{
		if(
			this->activeVideoReq->filename == filename && 
			this->activeVideoReq->status == VideoRequest::Status::StreamingOut)
		{
			return this->activeVideoReq;
		}
	}

	this->_CloseVideo_NoMutex();

	// We may have a frame immediately write, but we'll open it
	// tenatively first.
	this->activeVideoReq = VideoRequest::MakeRequest(0, 0, this->cameraId, filename);
	this->activeVideoReq->status = VideoRequest::Status::Requested;
	{
		// If we have a frame, that will set the size parameters.
		std::lock_guard<std::mutex> imgGuard(imageAccess);
		if(!curCamFrame->empty())
			this->_DumpImageToVideofile(*this->curCamFrame);
	}
	return this->activeVideoReq;
}

bool ManagedCam::_CloseVideo_NoMutex()
{
	if(this->videoWrite.isOpened())
		this->videoWrite.release();

	if(this->activeVideoReq == nullptr)
		return false;

	this->activeVideoReq->status = VideoRequest::Status::Closed;
	this->activeVideoReq = nullptr;

	return true;
}

bool ManagedCam::_DumpImageToVideofile(const cv::Mat& img)
{
	if(img.empty())
	{
		this->_CloseVideo_NoMutex();
		return false;
	}

	// No request of where to save it was set.
	if(this->activeVideoReq == nullptr)
		return false;

	// If the user requested the stream to be stopped, using the
	// request handle (via VideoRequest::RequestStop()).
	if(this->activeVideoReq->_reqStopped)
	{ 
		this->_CloseVideo_NoMutex();

		// Technically... it was handled properly... so I guess it's true.
		return true;
	}

	// If the videowrite has not been opened yet, open it locked
	// to the image size. Every image streamed to the video will
	// now need to match the dimensions, or else it's considered
	// an error.
	if(!this->videoWrite.isOpened())
	{
		int mp4FourCC = cv::VideoWriter::fourcc('a', 'v', 'c', '1');
		this->videoWrite.open(this->activeVideoReq->filename, mp4FourCC, 30.0, img.size());
		if(!this->videoWrite.isOpened())
		{
			this->activeVideoReq->err = "Could not open requested file.";
			this->activeVideoReq->status = VideoRequest::Status::Error;
			this->_CloseVideo_NoMutex();
		}
		this->activeVideoReq->width		= img.size().width;
		this->activeVideoReq->height	= img.size().height;
		this->activeVideoReq->status	= VideoRequest::Status::StreamingOut;
	}
	else
	{
		// If contents have already been streamed, just make sure the dimensions
		// continue to be the same.
		if(
			this->activeVideoReq->width		!= img.size().width ||
			this->activeVideoReq->height	!= img.size().height )
		{
			this->activeVideoReq->err = "Closed when attempting to add misshapened image.";
			this->activeVideoReq->status = VideoRequest::Status::Error;
			this->_CloseVideo_NoMutex();
		}

		this->videoWrite.write(img);
	}
	return true;
}

void ManagedCam::_ClearImplementation(bool delCurrent, bool resetPollTy)
{
	if(this->currentImpl != nullptr)
	{
	}
	if(resetPollTy)
		this->pollType = VideoPollType::Deactivated;
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

bool ManagedCam::CloseVideo()
{
	std::lock_guard<std::mutex> guard(this->videoAccess);
	return this->_CloseVideo_NoMutex();
}

bool ManagedCam::IsRecordingVideo()
{
	std::lock_guard<std::mutex> guard(this->videoAccess);
	return this->videoWrite.isOpened();
}

std::string ManagedCam::VideoFilepath()
{
	std::lock_guard<std::mutex> guard(this->videoAccess);
	if(this->activeVideoReq == nullptr)
		return std::string();

	return this->activeVideoReq->filename;
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

			while( // Polling loop
				this->currentImpl->PollType() == pollTy &&
				this->currentImpl->IsValid())
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
				}

				this->msInterval = swFPS.Milliseconds();
				int msLeft = swLoopSleep.MSLeft33();
				MSSleep(msLeft);
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
	this->conState = State::Shutdown;
}

bool ManagedCam::_FinalizeHandlingPolledImage(cv::Ptr<cv::Mat> ptr)
{
	if(ptr == nullptr)
		return false;

	if(ptr->empty())
		return false;

	//		PROCESS AND CACHE VIDEO
	// 
	//////////////////////////////////////////////////

	ptr = this->ProcessImage(ptr);

	if(ptr)
		this->SetCurrentFrame(ptr);

	//
	//		SAVE IMAGE REQUESTS
	//
	//////////////////////////////////////////////////
	// If there's anything in the snap requests, swap this empty
	// one with a copy of the requests and claim ownership.
	//
	// Possibly more overhead to always create the vector, but the
	// swap minimized the amount of time the mutex is active.
	std::vector<SnapRequest::SPtr> sptrSwap;
	{
		std::lock_guard<std::mutex> guardReqs(this->snapReqsAccess);
		std::swap(sptrSwap, this->snapReqs);
	}
	// Attempt to save file and report the success status back to 
	// the shared pointer.
	for(SnapRequest::SPtr snreq : sptrSwap)
	{
		if(cv::imwrite(snreq->filename, *ptr))
		{
			snreq->frameID = this->camFeedChanges;
			snreq->status = SnapRequest::Status::Filled;
		}
		else
		{
			// Not the most in-depth error message, but using OpenCV
			// this way doesn't give us too much grainularity.
			snreq->err = "Error attempting to save file.";
			snreq->status = SnapRequest::Status::Error;
		}
	}

	{
		//		SAVE VIDEO
		//
		//////////////////////////////////////////////////
		// If we're holding on to a non-null video request, it will be 
		// either waiting to stream, or already streaming - no misc or
		// error conditions.
		std::lock_guard<std::mutex> guardVideo(this->videoAccess);
		if(this->activeVideoReq != nullptr)
			this->_DumpImageToVideofile(*ptr);
	}

	return true;
}

bool ManagedCam::BootupPollingThread(int camIdx)
{
	// Once someone shuts down the camera system for the app,
	// for the given app session, that's it- it's kaputskies 
	// (by design) for the rest of the lifetime's app.
	if(this->_sentShutdown)
		return false;

	// If the camera stream is already running, there's nothing that
	// need booting.
	if(this->camStreamThread != nullptr)
		return false;

	this->camStreamThread = 
		new std::thread(
			[this, camIdx]
			{
				// This thread is expected to run until 
				// CamStreamMgr::Shutdown() is called at the end
				// of the app's lifetime.

				// Flag the command to start the video capturing for the thread
				this->ThreadFn(camIdx);
				this->_isShutdown = true;
			});

	return true;
}

cv::Ptr<cv::Mat> ManagedCam::ThresholdImage(cv::Ptr<cv::Mat> src)
{

	cv::Mat grey, cl, thresholded, blurred, edges, kernel, dialated, flooded, invert;

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

	/// Convert it to gray
	cv::cvtColor(*src, grey, cv::COLOR_RGBA2GRAY, 0);

	//equalize
	cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
	clahe->setClipLimit(2.7);
	clahe->apply(grey, cl);

	//make histogram
	calcHist(&cl, 1, channels, cv::Mat(), // do not use mask
		hist, 1, histSize, ranges,
		true, // the histogram is uniform
		false);

	// yen_thresholding
	int yen_threshold = Yen(hist);
	//std::cout << "Yen threshold : " << yen_threshold << "\n";
	cv::threshold(cl,
		thresholded,
		double(yen_threshold),
		255,
		cv::THRESH_TOZERO);
	//Note THRESH_TO_ZERO is only one option another, possibly better option is THRESH_BINARY

	//blur
	cv::medianBlur(thresholded, blurred, 7);

	//Note the next steps are expensive and possibly unnecesaary keeping them for completeness
	//edges
	cv::Canny(blurred, edges, 120, 120);

	//dialate
	kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3), cv::Point(1, 1));
	cv::dilate(edges, dialated, kernel);

	//flood
	flooded = dialated.clone();
	cv::floodFill(flooded, cv::Point(0, 0), cv::Scalar(255));

	//invert
	cv::bitwise_not(flooded, invert);
	cv::Mat* inverted = new cv::Mat(invert);
	return inverted;//Note might be worth changing this to blurred after changing thresholding ot use THRESH_BINARY

}

cv::Ptr<cv::Mat> ManagedCam::ProcessImage(cv::Ptr<cv::Mat> inImg)
{
	switch (this->camOptions.processing)
	{
	default:
		cvgAssert(false,"Unhandled processing switch");

	case ProcessingType::None:
		return inImg;

	case ProcessingType::yen_threshold:
		return ThresholdImage(inImg);
	}
}

void ManagedCam::_DeactivateStreamState(bool deactivateShould)
{
	if(deactivateShould)
		this->pollType = VideoPollType::Deactivated;

	this->_isStreamActive		= false;
	this->streamWidth			= -1;
	this->streamHeight			= -1;
	this->msInterval			= 0;
}

void ManagedCam::SetPoll(VideoPollType pollTy)
{
	this->pollType = pollTy;
}