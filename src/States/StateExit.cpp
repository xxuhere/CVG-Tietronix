#include "StateExit.h"
#include "../MainWin.h"

StateExit::StateExit(HMDOpApp* app, GLWin* view, MainWin* core)
	: BaseState(BaseState::AppState::Exit, app, view, core)
{
}

StateExit::~StateExit()
{
}

void StateExit::Draw(const wxSize& sz)
{
	// Clear color is a bit bright, so when the application
	// fully shuts down, the user can notice a transition to
	// full black.
	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

	this->mainFont.RenderFontCenter(
		"Shutting down application.", 
		UIVec2(sz.x / 2, sz.y / 2),
		false);

	this->mainFont.RenderFontCenter(
		"Please wait.", 
		UIVec2(sz.x / 2, sz.y / 2 + 30),
		false);
}

void StateExit::Update(double dt)
{
}

void StateExit::EnteredActive()
{
	// Simply entering this state will automate requesting
	// the application to close.	
	this->GetCoreWindow()->Close();
}

void StateExit::ExitedActive()
{
}

void StateExit::Initialize()
{
	this->mainFont = FontMgr::GetInstance().GetFont(24);
}

void StateExit::ClosingApp()
{
}

void StateExit::OnKeydown(wxKeyCode key)
{
}

void StateExit::OnMouseDown(int button, const wxPoint& pt)
{
}