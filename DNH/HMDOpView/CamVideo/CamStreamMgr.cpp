#include "CamStreamMgr.h"
#include <thread>
#include <mutex>
#include "ManagedCam.h"
#include "../Utils/multiplatform.h"

CamStreamMgr CamStreamMgr::_inst;

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
	// Note: this is just the construction of the object. Remember
	// it's not up-and-running until it's initialized with
	// BootConnectionToCamera() by outside code.
}

CamStreamMgr::~CamStreamMgr()
{
	// Shutdown should only be called once, but it's designed
	// to be safe to call multiple times and ignore further
	// requests.
	this->Shutdown();
}

bool CamStreamMgr::BootConnectionToCamera(
	int camCt, 
	VideoPollType allDefaultPoll)
{
	if(camCt <= 0)
		return false;

	std::vector<cvgCamFeedSource> delegVec;
	// Make a form we can delegate to the finer-control overload.
	for(int i = 0; i < camCt; ++i)
	{
		cvgCamFeedSource src;
		src.camIndex = i;
		src.defPoll = allDefaultPoll;
	}

	return this->BootConnectionToCamera(delegVec);
}

bool CamStreamMgr::BootConnectionToCamera(const std::vector<cvgCamFeedSource>& sources)
{
	if(sources.size() == 0)
		return false;

	std::lock_guard<std::mutex> guard(this->camAccess);

	// Initializing threads to run in the background can only
	// be allowed if there aren't already background threads
	// running.
	if(!this->cams.empty())
		return false;

	for(int i = 0; i < sources.size(); ++i)
	{
		VideoPollType usedPollTy = sources[i].GetUsedPoll();
		std::cout << "Creating poll of type " << to_string(usedPollTy) << std::endl;
		ManagedCam* newMc = new ManagedCam(usedPollTy, i, sources[i]);


		this->cams.push_back(newMc);
		newMc->BootupPollingThread(i);
	}
	return true;
}

cv::Ptr<cv::Mat> CamStreamMgr::GetCurrentFrame(int idx)
{
	std::lock_guard<std::mutex> guard(this->camAccess);
	if(this->cams.empty())
		return cv::Ptr<cv::Mat>();

	return this->cams[idx]->GetCurrentFrame();
}

long long CamStreamMgr::GetCameraFeedChanges(int idx)
{
	std::lock_guard<std::mutex> guard(this->camAccess);
	if(this->cams.empty())
		return -1;

	return this->cams[idx]->camFeedChanges;
}

void CamStreamMgr::SetPollType(int idx, VideoPollType pty)
{
	std::lock_guard<std::mutex> guard(this->camAccess);
	if(this->cams.empty())
		return;

	this->cams[idx]->SetPoll(pty);
}

void CamStreamMgr::ClearAllSnapshotRequests()
{
	std::lock_guard<std::mutex> guard(this->camAccess);

	for(ManagedCam* mc : this->cams)
		mc->ClearSnapshotRequests();
}

SnapRequest::SPtr CamStreamMgr::RequestSnapshot(int idx, const std::string& filename)
{
	std::lock_guard<std::mutex> guard(this->camAccess);

	return this->cams[idx]->RequestSnapshot(filename);
}

void CamStreamMgr::ClearSnapshotRequests(int idx)
{
	std::lock_guard<std::mutex> guard(this->camAccess);
	if(this->cams.empty())
		return;

	this->cams[idx]->ClearSnapshotRequests();
}

VideoRequest::SPtr CamStreamMgr::RecordVideo(int idx, const std::string& filename)
{
	std::lock_guard<std::mutex> guard(this->camAccess);
	if(this->cams.empty())
		return VideoRequest::MakeError("__invalidstate__");

	return this->cams[idx]->OpenVideo(filename);
}

bool CamStreamMgr::StopRecording(int idx)
{
	std::lock_guard<std::mutex> guard(this->camAccess);
	if(this->cams.empty())
		return false;

	return this->cams[idx]->CloseVideo();
}

bool CamStreamMgr::IsRecording(int idx)
{
	std::lock_guard<std::mutex> guard(this->camAccess);
	if(this->cams.empty())
		return false;

	return this->cams[idx]->IsRecordingVideo();
}

bool CamStreamMgr::IsThresholded(int idx)
{
	std::lock_guard<std::mutex> guard(this->camAccess);
	if(this->cams.empty())
		return false;

	return this->cams[idx]->IsThresholded();
}

std::string CamStreamMgr::RecordingFilename(int idx)
{
	std::lock_guard<std::mutex> guard(this->camAccess);
	if(this->cams.empty())
		return std::string();

	return this->cams[idx]->VideoFilepath();
}

int CamStreamMgr::GetMSFrameTime(int idx)
{
	std::lock_guard<std::mutex> guard(this->camAccess);
	if(this->cams.empty())
		return -1;

	return this->cams[idx]->msInterval;
}

int CamStreamMgr::GetStreamFrameCt(int idx)
{
	std::lock_guard<std::mutex> guard(this->camAccess);
	if(this->cams.empty())
		return -1;

	return this->cams[idx]->streamFrameCt;
}

ManagedCam::State CamStreamMgr::GetState(int idx) 
{ 
	std::lock_guard<std::mutex> guard(this->camAccess);
	if(this->cams.empty())
		return ManagedCam::State::Unknown;

	return this->cams[idx]->GetState();
}

bool CamStreamMgr::Shutdown()
{
	std::lock_guard<std::mutex> guard(this->camAccess);

	for(ManagedCam* mc : this->cams)
		mc->ShutdownThread();

	// The threads have been signaled to shutdown, but they still
	// take a while to shutdown. We wait for them to finish before
	// joining.
	for(ManagedCam* mc : this->cams)
	{ 
		while(!mc->_isShutdown)
			MSSleep(10);
	}

	for(ManagedCam* mc : this->cams)
		mc->_JoinThread();

	for(ManagedCam* mc : this->cams)
		delete mc;

	this->cams.clear();
	 return true;
}