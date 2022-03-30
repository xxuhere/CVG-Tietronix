#include "MainWin.h"
#include <wx/wx.h>
#include <wx/menu.h>
#include "HMDOpApp.h"
#include <vector>

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
	this->Show();
	
	// The process of making the window fullscreen is different
	// between WinOS and Linux. 
	// - For Windows, if ShowFullscreen() is used, a white border
	// along the outside edges may show.
	// - Fow Linux, if we change the window style of 0, it behaves
	// weird and won't fullscreen or draw correctly.
#if _WIN32
	this->SetWindowStyle(0);
	this->Maximize();
#else
	this->ShowFullScreen(true, wxFULLSCREEN_ALL);
#endif


	this->innerGLWin = new GLWin(this);

	wxBoxSizer* szrMain = new wxBoxSizer(wxVERTICAL);
	szrMain->Add(this->innerGLWin, 1, wxEXPAND, 0);
	this->SetSizer(szrMain);
	this->Layout();
	this->innerGLWin->SetGLCurrent();

	this->PopulateStates();
	this->States_Initialize();
	this->ChangeState(BaseState::AppState::Intro);

	std::vector<wxAcceleratorEntry> entries;
	entries.push_back(wxAcceleratorEntry(wxACCEL_ALT, WXK_F4, wxID_EXIT));

	// Setup Alt+F4 to exit the app (we lost that when we stripped the app
	// to be bare-bone since that keyboard accelerator was originally handled
	// by adding the menu item.
	wxAcceleratorTable accelTable(entries.size(), &entries[0]);
	this->SetAcceleratorTable(accelTable);

	this->innerGLWin->SetFocus();

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
	this->States_AppShutdown();
	this->Close( true );
}
