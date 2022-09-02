#include "CamStreamMgr.h"
#include <thread>
#include <mutex>
#include "ManagedCam.h"
#include "ManagedComposite.h"
#include "../Utils/multiplatform.h"
#include <iostream>
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

IManagedCam* CamStreamMgr::_GetIManaged(int idx)
{
	switch(idx)
	{
	case SpecialCams::Composite:
		return this->composite;
		break;

	case SpecialCams::ErrorCode:
		return nullptr;
		break;
	}

	// Below is a duplicate of _GetManaged().
	if(this->cams.empty())
		return nullptr;

	if( idx < 0 || idx >= this->cams.size())
		return nullptr;

	return this->cams[idx];
}

ManagedCam* CamStreamMgr::_GetManaged(int idx)
{
	if(this->cams.empty())
		return nullptr;

	if( idx < 0 || idx >= this->cams.size())
		return nullptr;

	return this->cams[idx];
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

void CamStreamMgr::ValidateSourcePlatforms(const std::vector<cvgCamFeedSource>& sources)
{
	// Validate all poll types before using, split out an error if the
	// poll type being used is invalid for the platform the program
	// is currently running on.
	// https://github.com/Achilefu-Lab/CVG-Tietronix/issues/4
	//
	bool anyPlatformIssues = false;
	for(int i = 0; i < sources.size(); ++i)
	{
		VideoPollType usedPollTy = sources[i].GetUsedPoll();
		// For each type, detect if on the wrong platform - any 
		switch(usedPollTy)
		{
		case VideoPollType::MMAL:
		case VideoPollType::OpenCVUSB_Named:
#ifdef WIN32
			std::cerr << "ERROR: For video feed " << i << ": Cannot use " << to_string(usedPollTy) << " on Windows." << std::endl;
			anyPlatformIssues = true;
#endif
			break;

		case VideoPollType::OpenCVUSB_Idx:
#if IS_RPI
			std::cerr << "ERROR: For video feed " << i << ": Cannot use " << to_string(usedPollTy) << " on RPi/Linux." << std::endl;
			anyPlatformIssues = true;
#endif
			break;

		}
	}
	if(anyPlatformIssues)
	{
		std::stringstream sstrm;

		sstrm << "ERROR: Invalid polling type used for";
#if IS_RPI
		sstrm << "RaspberryPi/Linux";
#elif WIN32
		sstrm << "Windows";
#else
		sstrm << "UNKNOWN_PLATFORM";
#endif
		sstrm << ". Fix by changing AppOptions.json and then restart. Shutting down application." << std::endl;

		std::string err = sstrm.str();

		// We may need to find something specific for WindowsOS to let the users
		// see the error. MessageBox was tried, but was not modal.
		std::cerr << err.c_str();

		exit(1);
	}
}

bool CamStreamMgr::BootConnectionToCamera(const std::vector<cvgCamFeedSource>& sources)
{
	if(sources.size() == 0)
		return false;

	ValidateSourcePlatforms(sources);

	std::lock_guard<std::mutex> guard(this->camAccess);

	if(this->composite == nullptr)
	{
		this->composite = new ManagedComposite();
		this->composite->BootupPollingThread(SpecialCams::Composite);
	}

	// Initializing threads to run in the background can only
	// be allowed if there aren't already background threads
	// running.
	if(!this->cams.empty())
		return false;

	//////////////////////////////////////////////////
	

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
	IManagedCam* imc = this->_GetIManaged(idx);
	if(imc == nullptr)
		return cv::Ptr<cv::Mat>();

	return imc->GetCurrentFrame();
}

long long CamStreamMgr::GetCameraFeedChanges(int idx)
{
	std::lock_guard<std::mutex> guard(this->camAccess);
	IManagedCam* imc = this->_GetIManaged(idx);
	if(imc == nullptr)
		return -1;

	return imc->camFeedChanges;
}

void CamStreamMgr::SetPollType(int idx, VideoPollType pty)
{
	std::lock_guard<std::mutex> guard(this->camAccess);
	ManagedCam* mc = this->_GetManaged(idx);
	if(mc == nullptr)
		return;

	mc->SetPoll(pty);
}

void CamStreamMgr::ClearAllSnapshotRequests()
{
	std::lock_guard<std::mutex> guard(this->camAccess);

	for(ManagedCam* mc : this->cams)
		mc->ClearSnapshotRequests();
}

SnapRequest::SPtr CamStreamMgr::RequestSnapshot(
	int idx, 
	const std::string& filename, 
	SnapRequest::ProcessType procType)
{
	std::lock_guard<std::mutex> guard(this->camAccess);

	IManagedCam* imc = this->_GetIManaged(idx);
	if(imc == nullptr)
		return SnapRequest::MakeError("Could not find requested camera stream", filename);

	return imc->RequestSnapshot(filename, procType);
}

std::vector<SnapRequest::SPtr> CamStreamMgr::RequestSnapshotAll(const std::string& filenameBase)
{
	std::lock_guard<std::mutex> guard(this->camAccess);

	std::vector<SnapRequest::SPtr> ret;

	std::vector<IManagedCam*> allCams;
	for(int i = 0; i < this->cams.size(); ++i)
		allCams.push_back(this->cams[i]);

	if(this->composite != nullptr)
		allCams.push_back(this->composite);

	for(IManagedCam* mc : allCams)
	{
		std::string strIdx = mc->GetStreamName();
		std::string fullFilename = filenameBase + "_" + strIdx + "RAW";
		SnapRequest::SPtr camSnapRaw = mc->RequestSnapshot(fullFilename, SnapRequest::ProcessType::Cannot);

		if(camSnapRaw)
			ret.push_back(camSnapRaw);

		if(mc->UsesImageProcessingChain())
		{
			std::string fullFilename = filenameBase + "_" + strIdx + "IMPROC";
			SnapRequest::SPtr camSnapIProc = mc->RequestSnapshot(fullFilename, SnapRequest::ProcessType::HasTo);
			if(camSnapIProc)
				ret.push_back(camSnapIProc);
		}

	}

	return ret;
}

void CamStreamMgr::ClearSnapshotRequests(int idx)
{
	std::lock_guard<std::mutex> guard(this->camAccess);
	IManagedCam* imc = this->_GetIManaged(idx);
	if(imc == nullptr)
		return;

	imc->ClearSnapshotRequests();
}

VideoRequest::SPtr CamStreamMgr::RecordVideo(int idx, const std::string& filename)
{
	std::lock_guard<std::mutex> guard(this->camAccess);
	IManagedCam* imc = this->_GetIManaged(idx);
	if(imc == nullptr)
		return VideoRequest::MakeError("__invalidstate__");

	return imc->OpenVideo(filename);
}

bool CamStreamMgr::StopRecording(int idx)
{
	std::lock_guard<std::mutex> guard(this->camAccess);
	IManagedCam* imc = this->_GetIManaged(idx);
	if(imc == nullptr)
		return false;

	return imc->CloseVideo();
}

bool CamStreamMgr::IsRecording(int idx)
{
	std::lock_guard<std::mutex> guard(this->camAccess);
	IManagedCam* imc = this->_GetIManaged(idx);
	if(imc == nullptr)
		return false;

	return imc->IsRecordingVideo();
}

bool CamStreamMgr::IsThresholded(int idx)
{
	std::lock_guard<std::mutex> guard(this->camAccess);
	ManagedCam* mc = this->_GetManaged(idx);
	if(mc == nullptr)
		return false;

	return mc->IsThresholded();
}

std::string CamStreamMgr::RecordingFilename(int idx)
{
	std::lock_guard<std::mutex> guard(this->camAccess);
	IManagedCam* imc = this->_GetIManaged(idx);
	if(imc == nullptr)
		return std::string();

	return imc->VideoFilepath();
}

int CamStreamMgr::GetMSFrameTime(int idx)
{
	std::lock_guard<std::mutex> guard(this->camAccess);
	IManagedCam* imc = this->_GetIManaged(idx);
	if(imc == nullptr)
		return -1;

	return imc->msInterval;
}

int CamStreamMgr::GetStreamFrameCt(int idx)
{
	std::lock_guard<std::mutex> guard(this->camAccess);
	IManagedCam* imc = this->_GetIManaged(idx);
	if(imc == nullptr)
		return -1;

	return imc->streamFrameCt;
}

ProcessingType CamStreamMgr::GetProcessingType(int idx)
{
	std::lock_guard<std::mutex> guard(this->camAccess);
	ManagedCam* mc = this->_GetManaged(idx);
	if(mc == nullptr)
		return ProcessingType::None;

	return mc->GetProcessingType();
}

bool CamStreamMgr::SetProcessingType(int idx, ProcessingType pt)
{
	std::lock_guard<std::mutex> guard(this->camAccess);
	ManagedCam* mc = this->_GetManaged(idx);
	if(mc == nullptr)
		return false;

	return mc->SetProcessingType(pt);
}

ManagedCam::State CamStreamMgr::GetState(int idx) 
{ 
	std::lock_guard<std::mutex> guard(this->camAccess);
	IManagedCam* imc = this->_GetIManaged(idx);
	if(imc == nullptr)
		return ManagedCam::State::Unknown;

	return imc->GetState();
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

	// The composite should be handled AFTER all the individual
	// video feeds have been finished, so we know there's no risk
	// that they'll continue to add frames to the composite while
	// it's trying to shutdown.

	if(this->composite != nullptr)
	{
		this->composite->ShutdownThread();

		while(!this->composite->_isShutdown)
			MSSleep(10);

		this->composite->_JoinThread();

		delete this->composite;
		this->composite = nullptr;
	}

	this->cams.clear();
	 return true;
}


float CamStreamMgr::GetFloat(int id, StreamParams paramid)
{
	std::lock_guard<std::mutex> guard(this->camAccess);
	IManagedCam* imc = this->_GetIManaged(id);
	if(imc == nullptr)
		return -1.0f;

	return imc->GetFloat(paramid);
}

bool CamStreamMgr::SetFloat(int id, StreamParams paramid, float value)
{
	std::lock_guard<std::mutex> guard(this->camAccess);
	IManagedCam* imc = this->_GetIManaged(id);
	if(imc == nullptr)
		return false;

	return imc->SetFloat(paramid, value);
}

void CamStreamMgr::SetSnapCaption(int id, const std::string& caption)
{
	std::lock_guard<std::mutex> guard(this->camAccess);
	IManagedCam* imc = this->_GetIManaged(id);
	if(imc == nullptr)
		return;

	imc->SetSnapCaption(caption);
}

void CamStreamMgr::SetAllSnapCaption(const std::string& caption)
{
	std::lock_guard<std::mutex> guard(this->camAccess);

	for(int i = 0; i < this->cams.size(); ++i)
		this->cams[i]->SetSnapCaption(caption);

	if(this->composite != nullptr)
		this->composite->SetSnapCaption(caption);
}
