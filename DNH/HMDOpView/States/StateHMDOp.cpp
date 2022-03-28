#include "StateHMDOp.h"
#include "StateIncludes.h"

StateHMDOp::StateHMDOp(HMDOpApp* app, GLWin* view, MainWin* core)
	: BaseState(BaseState::AppState::MainOp, app, view, core)
{
}

void StateHMDOp::Draw(const wxSize& sz)
{
}

void StateHMDOp::Update()
{
}

void StateHMDOp::EnteredActive()
{
}

void StateHMDOp::ExitedActive() 
{
}

void StateHMDOp::Initialize() 
{
}

void StateHMDOp::ClosingApp() 
{
}

StateHMDOp::~StateHMDOp()
{
}