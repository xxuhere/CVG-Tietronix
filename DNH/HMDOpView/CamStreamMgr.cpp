#include "CamStreamMgr.h"
#include <opencv2/imgcodecs.hpp>

CamStreamMgr CamStreamMgr::_inst;

#if WIN32
	#include <windows.h>
	void MSSleep(int ms) { Sleep(ms); }
#else
	#include <unistd.h>
	void MSSleep(int ms) { usleep(ms); }
#endif

CamStreamMgr& CamStreamMgr::GetInstance()
{
	return _inst;
}

bool CamStreamMgr::ShutdownMgr()
{
	return _inst.Shutdown();
}

CamStreamMgr::CamStreamMgr()
{
}

CamStreamMgr::~CamStreamMgr()
{
}

void CamStreamMgr::ThreadFn()
{
	this->_isStreamActive = false;

	// This will be the loop for the thread for the lifetime of the app
	// once booted. This should NOT be confused with the polling loop
	// of the camera, which will be an inner loop.
	while(this->_sentShutdown == false)
	{
		
		if(this->_shouldStreamBeActive)
		{
			if(this->testingMode == false)
			{
				this->conState = State::Connecting;
				this->_isStreamActive = false;

				cv::VideoCapture vc;
				if(vc.open(0, 0) == true)
				{
					this->_isStreamActive = true;
					vc.set(cv::CAP_PROP_BUFFERSIZE, 1);

					this->streamWidth	= (int)vc.get(cv::CAP_PROP_FRAME_WIDTH);
					this->streamHeight	= (int)vc.get(cv::CAP_PROP_FRAME_HEIGHT);

					// Camera frames polling loop
					while(
						vc.isOpened() && 
						this->_sentShutdown == false &&
						this->_shouldStreamBeActive == true &&
						this->testingMode == false)
					{
						this->conState = State::Polling;

						cv::Mat* pmat = new cv::Mat();
						cv::Ptr<cv::Mat> ptr(pmat);

						vc >> *pmat;
						if(!pmat->empty())
							this->SetCurrentFrame(ptr);

						MSSleep(30);
					}
				}
				this->_DeactivateStreamState();
			}
			if(this->testingMode == true)
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
					this->_shouldStreamBeActive == true &&
					this->testingMode == true)
				{
					if(!decoy->empty())
						this->SetCurrentFrame(sprtDecoy);

					MSSleep(30);
				}

				this->_DeactivateStreamState();
			}			
		}
		else
		{
			this->conState = State::Idling;
			MSSleep(500);
		}
	}

	this->conState = State::Shutdown;
}

cv::Ptr<cv::Mat> CamStreamMgr::ProcessImage(cv::Ptr<cv::Mat> inImg)
{
	return inImg;
}

void CamStreamMgr::BootConnectionToCamera()
{
	// Once sometime tries to shutdown the camera system for the app,
	// for the given app session, that's it- it's kaputskies for the 
	// rest of the lifetime's app.
	if(this->_sentShutdown)
		return;

	// If the camera stream is already running, there's nothing that
	// need booting.
	if(this->camStreamThread != nullptr)
		return;

	this->camStreamThread = 
		new std::thread(
			[this]
			{
				// This thread is expected to run until 
				// CamStreamMgr::Shutdown() is called at the end
				// of the app's lifetime.

				// Flag the command to start the video capturing for the thread
				this->_shouldStreamBeActive = true;
				this->ThreadFn();
				this->_isShutdown = true;
			});
}

cv::Ptr<cv::Mat> CamStreamMgr::GetCurrentFrame()
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

bool CamStreamMgr::SetCurrentFrame(cv::Ptr<cv::Mat> mat)
{
	std::lock_guard<std::mutex> guard(this->imageAccess);
	this->curCamFrame = mat;
	++this->camFeedChanges;
	return true;
}

void CamStreamMgr::ToggleTesting()
{
	this->testingMode = !this->testingMode;
	this->_shouldStreamBeActive = true;
}

void CamStreamMgr::_DeactivateStreamState(bool deactivateShould)
{
	if(deactivateShould)
		this->_shouldStreamBeActive = false;
	this->_isStreamActive		= false;
	this->streamWidth			= -1;
	this->streamHeight			= -1;
}

bool CamStreamMgr::Shutdown()
{
	// We should only send ShutdownMgr and perform
	// the inner operations once per app session, and
	// only when the app is shutting down.
	if(this->_sentShutdown == false)
		return false;

	if(this->_isShutdown == false)
		return false;

	this->_sentShutdown = true;
	this->_shouldStreamBeActive = false;

	if(this->camStreamThread != nullptr)
	{
		this->_sentShutdown = true;
		this->_shouldStreamBeActive = false;
		this->camStreamThread->join();
		delete this->camStreamThread;
		this->camStreamThread = nullptr;
	}

	return true;
}