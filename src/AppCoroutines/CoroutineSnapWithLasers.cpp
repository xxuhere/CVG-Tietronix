#include "CoroutineSnapWithLasers.h"
#include "../Utils/TimeUtils.h"
#include "../CamVideo/CamStreamMgr.h"

CoroutineSnapWithLasers::CoroutineSnapWithLasers(
	MainWin* app, 
	GLWin* win,
	const std::string& phaseName)
	: cvgCoroutine(
		"Study Snapshot", 
		"Take a snapshot of all cameras with the laser both on and off")
{
	this->mainApp = app;
	this->glWin = win;

	this->phaseName = phaseName;
}

bool CoroutineSnapWithLasers::ImplStart()
{
	this->step = 0;

	// TODO: Figure out the suite capabilites of the expected 
	// laser hardware and the parameters that define it.
	this->laserOnAtStart = 
		this->mainApp->hwLaser->intensityNIR > 0.5f;

	return true;
}

bool CoroutineSnapWithLasers::ImplEnd()
{
	for(SnapRequest::SPtr snapPtr : this->queuedSnaps)
		snapPtr->Cancel();

	this->queuedSnaps.clear();

	if(this->laserOnAtStart)
	{
		// TODO: May need to be more accurate about this since the lights
		// are currently represented as a continuous float instead of
		// a binary value.
		this->mainApp->hwLaser->SetLight(LaserSys::Light::NIR, 1.0f);
	}
	else
	{
		this->mainApp->hwLaser->SetLight(LaserSys::Light::NIR, 0.0f);
	}
	return true;
}

bool CoroutineSnapWithLasers::ImplStep()
{
	switch(this->step)
	{
		
	case 0:	// Set the laser off
		this->mainApp->IncrSnapCounter();
		this->mainApp->hwLaser->HideNIR();
		++this->step;
		return true;

	case 1:	// Verify laser off
		++this->step;
		// NOP
		return true;

	case 2:	// Request pictures
		this->RequestSnapAll("LasOff");
		++this->step;
		return true;

	case 3: // Await pictures
		// See if anything is request and no fullfilled yet, if there are any,
		// then we need to still wait for them to be handled - so we exit before
		// incrementing the step counter.
		if(this->AwaitingOnAnySnapRequests())
			return true;
		
		++this->step;
		return true;

	case 4: // Set laser on
		this->mainApp->hwLaser->ShowNIR();
		++this->step;
		return true;

	case 5: // Verify laser on
		// NOP
		++this->step;
		return true;

	case 6: // Request pictures
		this->RequestSnapAll("LasOn");
		++this->step;
		return true;

	case 7: // Await pictures
		if(this->AwaitingOnAnySnapRequests())
			return true;

		++this->step;
		return true;

	case 8: // Reset pictures
		++this->step;
		return true;

	default:
		return false;
	}
}

bool CoroutineSnapWithLasers::ImplDispose()
{
	return true;
}

bool CoroutineSnapWithLasers::RequestSnapAll(const std::string& baseName)
{
	std::string folderLoc = this->mainApp->EnsureAndGetCapturesFolder();
	if(folderLoc.empty())
	{ 
		//queuedSnaps.push_back( SnapRequest::MakeError("Could not allocate capture session folder", "") );
		return false;
	}

	CamStreamMgr & camMgr = CamStreamMgr::GetInstance();
	// Build snapshot image filename
	std::stringstream sstrmFilebase;

	sstrmFilebase << folderLoc << "/Snap_" << FileDateTimeNow() << "_" << phaseName << "_" << baseName << "_" << this->mainApp->GetSnapCounter();
	std::string filebase = sstrmFilebase.str();

	std::vector<SnapRequest::SPtr> mgrRet = camMgr.RequestSnapshotAll(filebase);
	bool any = false;
	for(int i = 0; i < mgrRet.size(); ++i)
	{ 
		if(!mgrRet[i])
			continue;

		any = true;
		this->queuedSnaps.push_back(mgrRet[i]);
	}
	if(any)
		this->mainApp->PlayAudio_CameraSnap();

	return true;
}

bool CoroutineSnapWithLasers::AwaitingOnAnySnapRequests()
{
	for(SnapRequest::SPtr req : this->queuedSnaps)
	{
		if(req->GetStatus() == SnapRequest::Status::Requested)
			return true;
	}

	return false;
}