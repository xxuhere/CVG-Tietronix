// There's an issue here where items in wx and DCMTK have collisions. One of
// the big issues is that they both have a ssize_t typedef. For now it's tested
// to work, and we can get it to compile for both Windows and Linux if we swap
// the order of the headers on a per-platform basis.
// (wleu 07/28/2022)

#if WIN32
	// WARNING: wx.h must appear before DCMTK stuff or else the typedef for
	// ssize_t creates a compile error.
	#include <wx/wx.h>
	#include "DicomUtils/DicomInjectorSet.h"
#else
	// WARNING: Dicom related header files must appear before wxWidgets, to avoid
	// wxWidgets polluting Dicom stuff with UNICODE requirements.
	#include "DicomUtils/DicomInjectorSet.h"
	#include <wx/wx.h>
#endif

#include <wx/menu.h>
#include <wx/filename.h>

#include "MainWin.h"
#include "HMDOpApp.h"
#include <vector>
#include <sstream>
#include "Utils/cvgAssert.h"
#include <dcmtk/dcmdata/dcdeftag.h>

#include "States/StateIntro.h"
#include "States/StateInitCameras.h"
#include "States/StateHMDOp.h"
#include "States/StateExit.h"

#if IS_RPI
#include "Hardware/FauxMouse.h"
#endif

std::string FileDateTimeNow()
{
	wxDateTime dt = wxDateTime::Now();
	std::stringstream retStream;

	retStream << 
		dt.GetYear() << 
		std::setfill('0') << std::setw(2) << (dt.GetMonth() + 1) << 
		std::setfill('0') << std::setw(2) << dt.GetDay() << 
		std::setfill('0') << std::setw(2) << dt.GetHour() << 
		std::setfill('0') << std::setw(2) << dt.GetMinute() << 
		std::setfill('0') << std::setw(2) << dt.GetSecond();

	return retStream.str();
}

wxBEGIN_EVENT_TABLE(MainWin, wxFrame)
	EVT_MENU		(wxID_EXIT,  MainWin::OnExit)
	EVT_MENU		(wxID_SAVE,  MainWin::OnAccelerator_SaveCurOptions)
	EVT_SET_FOCUS	(MainWin::OnFocus			)
	EVT_SIZE		(MainWin::OnResize			)	
wxEND_EVENT_TABLE()

MainWin::MainWin(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(NULL, wxID_ANY, title, pos, size)
{
	this->InitializeSession();

	this->cameraSnap.Create("Assets/Audio/camera-13695.wav");
	if(!this->cameraSnap.IsOk())
		wxMessageBox("Could not load camera snap audio", "Audio Err");

	this->doubleBeep.Create("Assets/Audio/DoubleBeepAtn.wav");

	//
	//		GRAPHICS RENDERING RESOURCES
	//
	//////////////////////////////////////////////////
	this->innerGLWin = new GLWin(this);

	wxBoxSizer* szrMain = new wxBoxSizer(wxVERTICAL);
	szrMain->Add(this->innerGLWin, 1, wxEXPAND, 0);
	this->SetSizer(szrMain);
	this->Layout();
	this->innerGLWin->SetGLCurrent();
	// From here, we deffer a lot of initialization until
	// the first draw frame in GLWin::OnPaint().

	//////////////////////////////////////////////////

	this->ReloadAppOptions();

	//
	//		HARDWARE INIT AND MANAGEMENT	
	//
	//////////////////////////////////////////////////
	// Add all known hardware that's going to be used.
	// We keep references to we have typed access, but also we 
	// can now leave cleanup and management to the manager.
	this->hwLaser = new LaserSys();
	this->hmgr.Add(this->hwLaser);

#if IS_RPI
	// Faux-Mouse will use GPIO pins to emulate the mouse
	// behind the scenes on the RPi. Comment out adding it
	// to disable it.

	//this->hmgr.Add(new FauxMouse(this->innerGLWin));
#endif

	// When we actually initialize and validate, and how errors
	// are presented is something still TBD.
	this->hmgr.Initialize(std::cerr);
	this->hmgr.Validate(std::cerr);

	//
	//		KEYBOARD SHORTCUTS AND INIT FINALIZATIONS
	//
	//////////////////////////////////////////////////
	std::vector<wxAcceleratorEntry> entries;
	entries.push_back(wxAcceleratorEntry(wxACCEL_ALT, WXK_F4, wxID_EXIT));
	entries.push_back(wxAcceleratorEntry(wxACCEL_CTRL, 'S', wxID_SAVE));

	// Setup Alt+F4 to exit the app (we lost that when we stripped the app
	// to be bare-bone since that keyboard accelerator was originally handled
	// by adding the menu item.
	wxAcceleratorTable accelTable(entries.size(), &entries[0]);
	this->SetAcceleratorTable(accelTable);

	this->innerGLWin->SetFocus();

	// The process of making the window fullscreen is different
	// between WinOS and Linux. 
	// - For Windows, if ShowFullscreen() is used, a white border
	// along the outside edges may show.
	// - For Linux, if we change the window style of 0, it behaves
	// weird and won't fullscreen or draw correctly.
	if (this->innerGLWin->fullscreen)
	{
#if _WIN32
		this->SetWindowStyle(0);
		this->Maximize();
#else
		this->ShowFullScreen(true, wxFULLSCREEN_ALL);
#endif
	}

	// Register Dicom Injectors
	//
	//////////////////////////////////////////////////
	DicomInjectorSet& dicomInjSet = DicomInjectorSet::GetSingleton();
	dicomInjSet.AddInjectorRef(&this->opSession);

	// Final construction things
	//
	//////////////////////////////////////////////////

	this->Show();
}

bool MainWin::InitializeSession()
{
	const std::string& sessionLoc = wxGetApp().sessionLoc;


	// Try to load from file
	if(this->opSession.LoadFromFile(sessionLoc))
		return true;

	// If we can't load from file, take what we currently
	// have (probably a default object) and save that for next time.
	return this->opSession.SaveToFile(sessionLoc);
}

void MainWin::ClearWaitingSnaps()
{
	this->waitingSnaps.clear();
}

void MainWin::PerformMaintenenceCycle()
{
	this->ClearFinishedSnaps();
	this->ClearFinishedRecordings();
}

void MainWin::ClearFinishedSnaps()
{
	for(int i = this->waitingSnaps.size() - 1; i >= 0; --i)
	{
		if(this->waitingSnaps[i]->GetStatus() != SnapRequest::Status::Requested)
			this->waitingSnaps.erase(this->waitingSnaps.begin() + i);
	}
}

void MainWin::ClearFinishedRecordings()
{
	for(int i = this->recordingVideos.size() -1; i >= 0; --i)
	{

		switch(this->recordingVideos[i]->GetStatus())
		{
		case VideoRequest::Status::Error:
		case VideoRequest::Status::Closed:
		case VideoRequest::Status::Unknown:
			this->recordingVideos.erase(this->recordingVideos.begin() + i);
			break;
		}
	}
}

std::string MainWin::GetSessionsFolder() const
{
	return std::string("Captures/") + this->opSession.GenerateSessionPrefix();
}

std::string MainWin::EnsureAndGetCapturesFolder() const
{
	std::string folderLoc = this->GetSessionsFolder();
		

	// Check if the subfolders exist, if not, make them ahead of time.
	// NOTE: This is just creating the request. We may want to defer this
	// to when the requests are actually handled. 
	//
	// We don't rely on the return value of wxMkDir. For some reason it's
	// always failing even when it creates a valid folder. Instead we 
	// recheck the existence of the folder.
	if(!wxDirExists("Captures"))
	{
		wxMkDir("Captures", wxS_DIR_DEFAULT);
		if(!wxDirExists("Captures"))
			return "";
	}

	if(!wxDirExists(folderLoc))
	{
		wxMkDir(folderLoc, wxS_DIR_DEFAULT);
		if(!wxDirExists(folderLoc))
			return "";

		// If we need to create the session folder for the first time, also
		// drop in the AppOptions.json and Session.json for clerical reasons,
		// in case that data needs to be revisited after the operation.
		bool sessionWritten = false;
		bool appOptsWritten = false;
	
		if(wxFileExists("Session.json"))
			sessionWritten = wxCopyFile("Session.json", folderLoc + "/Session.json", false);

		const std::string& appOptsLoc = wxGetApp().appOptionsLoc;
		if(wxFileExists(appOptsLoc.c_str()))
		{
			// The AppOptions.json location is only "AppOptions.json" by default but
			// can be reassigned via commandline - and can be an absolute value. So
			// when taking its filename for the copy destination, we need to make sure
			// we're appending a filename and not an absolute path.
			wxFileName appOptsFilename(appOptsLoc);
			//
			appOptsWritten = wxCopyFile(appOptsLoc, folderLoc + "/" + appOptsFilename.GetFullName());
		}

		if(!sessionWritten)
			std::cerr << "Could not copy clerical version of sessions.json into session folder; invalid folder/file or file already existed." << std::endl;

		if(!appOptsWritten)
			std::cerr << "Could not copy clerical version of AppOptions.json into session folder; invalid folder/file or file already existed." << std::endl;


	}

	return folderLoc;
}

SnapRequest::SPtr MainWin::RequestSnap(
	int idx, 
	const std::string& prefix, 
	SnapRequest::ProcessType procType)
{
	CamStreamMgr & camMgr = CamStreamMgr::GetInstance();
	if(camMgr.GetState(idx) != ManagedCam::State::Polling)
		return SnapRequest::MakeError("_badreq_", "");
	

	std::string folderLoc = this->EnsureAndGetCapturesFolder();
	if(folderLoc.empty())
		return SnapRequest::MakeError("Could not allocate capture session folder", "");

	// Build snapshot image filename
	std::stringstream sstrmFilepath;
	sstrmFilepath << folderLoc << "/Snap_" << FileDateTimeNow() << "_" << this->opSession.sessionName << "_" <<  prefix << this->snapCtr << ".png";
	std::string filepath = sstrmFilepath.str();

	++this->snapCtr;

	SnapRequest::SPtr snreq = camMgr.RequestSnapshot(idx, filepath, procType);
	this->waitingSnaps.push_back(snreq);
	this->cameraSnap.Play();
	return snreq;
}

std::vector<SnapRequest::SPtr> MainWin::RequestSnapAll(const std::string& prefix)
{
	std::vector<SnapRequest::SPtr> ret;

	std::string folderLoc = this->EnsureAndGetCapturesFolder();
	if(folderLoc.empty())
	{ 
		ret.push_back( SnapRequest::MakeError("Could not allocate capture session folder", "") );
		return ret;
	}

	CamStreamMgr & camMgr = CamStreamMgr::GetInstance();
	// Build snapshot image filename
	std::stringstream sstrmFilebase;
	
	sstrmFilebase << folderLoc << "/Snap_" << FileDateTimeNow() << "_" <<  this->opSession.sessionName << "_" << prefix << "_" << this->snapCtr;
	std::string filebase = sstrmFilebase.str();

	++this->snapCtr;

	std::vector<SnapRequest::SPtr> mgrRet = camMgr.RequestSnapshotAll(filebase);
	bool any = false;
	for(int i = 0; i < mgrRet.size(); ++i)
	{ 
		if(!mgrRet[i])
			continue;

		any = true;
		this->waitingSnaps.push_back(mgrRet[i]);
	}
	if(any)
		this->cameraSnap.Play();

	return ret;

}

VideoRequest::SPtr MainWin::RecordVideo(int idx, const std::string& prefix)
{
	CamStreamMgr & camMgr = CamStreamMgr::GetInstance();
	if(camMgr.GetState(idx) != ManagedCam::State::Polling)
		return VideoRequest::MakeError("_badreq_");

	std::string folderLoc = this->EnsureAndGetCapturesFolder();
	if(folderLoc.empty())
		return VideoRequest::MakeError("Could not allocate capture session folder");

	// Build snapshot image filename
	std::stringstream sstrmFilepath;
	sstrmFilepath << folderLoc << "/Video_" << FileDateTimeNow() << "_" << prefix << this->opSession.sessionName << "_" << this->videoCtr << ".mkv";
	std::string filepath = sstrmFilepath.str();

	++this->videoCtr;

	// The video recording may cancel another video recording in another
	// file. We will handle that later by cleaning recordingVideos during the
	// regular maintenence cycle.
	VideoRequest::SPtr snreq = camMgr.RecordVideo(idx, filepath);
	this->recordingVideos.push_back(snreq);
	// A different audio should be played for video (and perhaps one when 
	// we detect the recording has been stopped - in the maintainence cycle).
	// But for now we'll recycle the camera snapping sound.
	this->cameraSnap.Play();
	return snreq;
}

bool MainWin::IsRecording(int idx)
{
	CamStreamMgr & camMgr = CamStreamMgr::GetInstance();
	return camMgr.IsRecording(idx);
}

bool MainWin::StopRecording(int idx)
{
	CamStreamMgr & camMgr = CamStreamMgr::GetInstance();
	return camMgr.StopRecording(idx);
}

void MainWin::ReloadAppOptions()
{
	this->innerGLWin->InitializeOptions();
}

void MainWin::InitializeAppStateMachine()
{
	this->PopulateStates();
	this->States_Initialize();
	this->ChangeState(BaseState::AppState::Intro);
}

void MainWin::PopulateStates()
{
	HMDOpApp& app = wxGetApp();

	// When adding/remove states two the systems, the classes
	// can be instanciated here, and the system should handle
	// the rest.
	//
	// - It's assumed there's an intro state that the app 
	// will be hardcoded to start on.
	// - Each state should have a different AppState enum value.
	// - Beyond the intro state, other states will only be visited
	// if transitioned to in the app - usually from another state.
	std::vector<BaseState*> statesToAdd =
	{
		new StateIntro(&app, this->innerGLWin, this),
		new StateInitCameras(&app, this->innerGLWin, this),
		new StateHMDOp(&app, this->innerGLWin, this),
		new StateExit(&app, this->innerGLWin, this)
	};

	for(BaseState* bs : statesToAdd)
	{
		cvgAssert(
			this->states.find(bs->GetState()) == this->states.end(),
			"Attemping to add multiple states mapped to the same id");

		this->states[bs->GetState()] = bs;
	}
}

void MainWin::States_Initialize()
{
	for(auto it : this->states)
		it.second->Initialize();
}

void MainWin::States_AppShutdown()
{
	if(this->curState != nullptr)
	{ 
		this->curState->ExitedActive();
		this->curState = nullptr;
	}

	for(auto it : this->states)
	{
		// This is usually called when the app is closing - if not,
		// it's all the same from the perspective of the states.
		it.second->ClosingApp();
		delete it.second;
	}
	this->states.clear();
}

bool MainWin::ChangeState(BaseState::AppState newState)
{
	if(this->curState != nullptr && this->curState->GetState() == newState)
		return false;

	auto it = this->states.find(newState);
	if(it == this->states.end())
		return false;

	BaseState* cur = it->second;
	// Let the current active state know it's no longer active.
	if(this->curState != nullptr)
		this->curState->ExitedActive();

	// The new active state.
	this->curState = cur;
	this->curState->EnteredActive();
	return true;
}

void MainWin::OnFocus(wxFocusEvent& evt)
{
	if(this->innerGLWin == nullptr)
		return;

	// If we get (keyboard) focus, deffer to child, because that's 
	// where all the important keyboard handling occurs.
	this->innerGLWin->SetFocus();
}

void MainWin::OnResize(wxSizeEvent& evt)
{
	wxSizer * szr = this->GetSizer();
	if(szr == nullptr)
		return;

	this->Layout();
}

void MainWin::OnExit(wxCommandEvent& event)
{
	this->innerGLWin->ReleaseStaticGraphicResources();
	this->States_AppShutdown();
	this->Close( true );
}

void MainWin::OnAccelerator_SaveCurOptions(wxCommandEvent& evt)
{
	std::cout << "Pressed shortcut key to overwrite AppOptions." << std::endl;
	this->innerGLWin->SaveOptions();
}