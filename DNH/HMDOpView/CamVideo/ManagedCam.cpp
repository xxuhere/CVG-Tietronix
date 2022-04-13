#include "ManagedCam.h"
#include <opencv2/imgcodecs.hpp>
#include "../Utils/multiplatform.h"

ManagedCam::ManagedCam(PollType pt, int idx)
{
	this->index = idx;
	this->pollType = pt;
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
		VideoRequest::SPtr noneRet = VideoRequest::MakeRequest(0, 0, this->index, filename);
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
	this->activeVideoReq = VideoRequest::MakeRequest(0, 0, this->index, filename);
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
		this->videoWrite.open(this->activeVideoReq->filename, mp4FourCC, 30, img.size());
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

bool ManagedCam::CloseVideo()
{
	std::lock_guard<std::mutex> guard(this->videoAccess);
	return this->_CloseVideo_NoMutex();
}

void ManagedCam::ThreadFn(int camIdx)
{
	this->_isStreamActive = false;

	// This will be the loop for the thread for the lifetime of the app
	// once booted. This should NOT be confused with the polling loop
	// of the camera, which will be an inner loop.
	while(this->_sentShutdown == false)
	{
		// if/else to see if we're any of the supported polling states
		//
		// The basic format will be
		//
		//	...
		//	else if(pollType == PollType::type)
		//	{
		//		pre-polling initialization
		//		while( 
		//			poll conditions &&
		//			pollType still equals PollType::type &&
		//			!_sentShutdown &&)
		//		{
		//			Poll & process single frame.
		//		}
		//		post-polling shutdown
		//	}
		if(this->pollType == PollType::OpenCVUSB)
		{
			this->conState = State::Connecting;
			this->_isStreamActive = false;

			cv::VideoCapture videoCapture;
			if(videoCapture.open(camIdx, 0) == true)
			{
				this->_isStreamActive = true;
				videoCapture.set(cv::CAP_PROP_BUFFERSIZE, 1);

				this->streamWidth	= (int)videoCapture.get(cv::CAP_PROP_FRAME_WIDTH);
				this->streamHeight	= (int)videoCapture.get(cv::CAP_PROP_FRAME_HEIGHT);

				// Camera frames polling loop
				while(
					videoCapture.isOpened() && 
					this->_sentShutdown == false &&
					this->pollType == PollType::OpenCVUSB)
				{
					this->conState = State::Polling;

					// Poll the current frame from OpenCV.
					cv::Mat* pmat = new cv::Mat();
					cv::Ptr<cv::Mat> ptr(pmat);
					videoCapture >> *pmat;

					_FinalizeHandlingPolledImage(ptr);
					MSSleep(30);
				}
			}
			this->_DeactivateStreamState();
		}
		if(this->pollType == PollType::Image)
		{
			// Simulate the feed with a static test image. This is useful in a
			// handful of situations:
			// - Deterministic and stable image to diagnose
			// - Iterating without having to wait for the webcam feed to initialize streaming.
			// - Testing the rest of the application without needing a webcam.
			this->_isStreamActive = true;
			this->conState = State::Polling;

			cv::Mat* decoy = new cv::Mat();
			*decoy = cv::imread("TestImg.png");
			cv::Ptr<cv::Mat> sprtDecoy(decoy);

			while(
				this->_sentShutdown == false &&
				this->pollType ==  PollType::Image)
			{
				_FinalizeHandlingPolledImage(sprtDecoy);
				MSSleep(30);
			}

			this->_DeactivateStreamState();
		}			
		else
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

cv::Ptr<cv::Mat> ManagedCam::ProcessImage(cv::Ptr<cv::Mat> inImg)
{
	return inImg;
}

void ManagedCam::_DeactivateStreamState(bool deactivateShould)
{
	if(deactivateShould)
		this->pollType = PollType::Deactivated;

	this->_isStreamActive		= false;
	this->streamWidth			= -1;
	this->streamHeight			= -1;
}

void ManagedCam::SetPoll(PollType pollTy)
{
	this->pollType = pollTy;
}