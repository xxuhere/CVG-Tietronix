#include "MainWin.h"
#include <wx/wx.h>
#include <wx/menu.h>
#include "HMDOpApp.h"
#include <vector>
#include <sstream>

#include "States/StateIntro.h"
#include "States/StateInitCameras.h"
#include "States/StateHMDOp.h"

wxBEGIN_EVENT_TABLE(MainWin, wxFrame)
	EVT_MENU		(wxID_EXIT,  MainWin::OnExit)
	EVT_SET_FOCUS	(MainWin::OnFocus			)
	EVT_SIZE		(MainWin::OnResize			)	
wxEND_EVENT_TABLE()

MainWin::MainWin(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(NULL, wxID_ANY, title, pos, size)
{
	this->cameraSnap.Create("Audio/camera-13695.wav");
	if(!this->cameraSnap.IsOk())
		wxMessageBox("Could not load camera snap audio", "Audio Err");

	this->opSession.SetName("William", "McGillicuddy", "Leu");
	this->opSession.SetSession("__TEST_SESSION__");

	//
	//		HARDWARE INIT AND MANAGEMENT	
	//
	//////////////////////////////////////////////////
	// Add all known hardware that's going to be used.
	// We keep references to we have typed access, but also we 
	// can now leave cleanup and management to the manager.
	this->hwLaser = new LaserSys();
	this->hmgr.Add(this->hwLaser);
	// When we actually initialize and validate, and how errors
	// are presented is something still TBD.
	this->hmgr.Initialize(std::cerr);
	this->hmgr.Validate(std::cerr);

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
	

	//
	//		KEYBOARD SHORTCUTS AND INIT FINALIZATIONS
	//
	//////////////////////////////////////////////////
	std::vector<wxAcceleratorEntry> entries;
	entries.push_back(wxAcceleratorEntry(wxACCEL_ALT, WXK_F4, wxID_EXIT));

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

	this->Show();
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

SnapRequest::SPtr MainWin::RequestSnap(int idx, const std::string& prefix)
{
	CamStreamMgr & camMgr = CamStreamMgr::GetInstance();
	if(camMgr.GetState(idx) != ManagedCam::State::Polling)
		return SnapRequest::MakeRequest("_badreq_");

	// Build snapshot image filename
	std::stringstream sstrmFilepath;
	sstrmFilepath << "Snap_" << prefix << this->opSession.sessionName << this->snapCtr << ".png";
	std::string filepath = sstrmFilepath.str();

	++this->snapCtr;

	SnapRequest::SPtr snreq = camMgr.RequestSnapshot(idx, filepath);
	this->waitingSnaps.push_back(snreq);
	this->cameraSnap.Play();
	return snreq;
}

VideoRequest::SPtr MainWin::RecordVideo(int idx, const std::string& prefix)
{
	CamStreamMgr & camMgr = CamStreamMgr::GetInstance();
	if(camMgr.GetState(idx) != ManagedCam::State::Polling)
		return VideoRequest::MakeError("_badreq_");

	// Build snapshot image filename
	std::stringstream sstrmFilepath;
	sstrmFilepath << "Video_" << prefix << this->opSession.sessionName << this->videoCtr << ".mkv";
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
	this->ReloadAppOptions();
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
		new StateHMDOp(&app, this->innerGLWin, this)
	};

	for(BaseState* bs : statesToAdd)
	{
		// TODO: Insert error handling? May not be necessary.
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
