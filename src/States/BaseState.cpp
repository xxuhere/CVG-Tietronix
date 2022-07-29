#include "BaseState.h"
#include "StateIncludes.h"

BaseState::C3F::C3F()
{}

BaseState::C3F::C3F(float r, float g, float b)
{
	this->r = r;
	this->g = g;
	this->b = b;
}

BaseState::BaseState(
	AppState state, 
	HMDOpApp* app, 
	GLWin* view, 
	MainWin* core)
{
	this->_state	= state;
	this->_app		= app;
	this->_view		= view;
	this->_core		= core;
}

BaseState::C3F BaseState::BackgroundColor()
{
	return C3F(0.0f, 0.0f, 0.0f);
}

BaseState::~BaseState()
{}

void BaseState::OnKeydown(wxKeyCode key)
{}

void BaseState::OnKeyup(wxKeyCode key)
{}

void BaseState::OnMouseDown(int button, const wxPoint& pt)
{}

void BaseState::OnMouseUp(int button, const wxPoint& pt)
{}

void BaseState::OnMouseMove(const wxPoint& pt)
{}

void BaseState::OnMouseWheel(int wheel, const wxPoint& pt)
{}