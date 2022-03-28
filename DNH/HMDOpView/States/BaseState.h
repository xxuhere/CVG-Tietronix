#pragma once

#include <wx/wx.h>
#include <wx/glcanvas.h>

class HMDOpApp;
class GLWin;
class MainWin;

class BaseState
{
public:
	enum AppState
	{
		Intro,
		InitCams,
		MainOp
	};

	struct C3F
	{
		float r;
		float g;
		float b;

		C3F();
		C3F(float r, float g, float b);
	};

private:
	// These are private, because subclasses
	// don't need to be resetting these.
	AppState	_state;
	HMDOpApp*	_app	= nullptr;
	GLWin*		_view	= nullptr;
	MainWin*	_core	= nullptr;

public:
	BaseState(AppState state, HMDOpApp* app, GLWin* view, MainWin* core);

	inline AppState GetState(){return this->_state;}
	inline HMDOpApp* GetApp() {return this->_app;}
	inline GLWin* GetView() {return this->_view;}
	inline MainWin* GetCoreWindow() {return this->_core;}

public:
	// TODO: Docstrings
	virtual void Draw(const wxSize& sz) = 0;
	virtual void Update() = 0;

	virtual void EnteredActive() = 0;
	virtual void ExitedActive() = 0;

	virtual void Initialize() = 0;
	virtual void ClosingApp() = 0;

	virtual C3F BackgroundColor();

	virtual void OnKeydown(wxKeyCode key);
	virtual void OnKeyup(wxKeyCode key);
	virtual void OnMouseDown(int button, const wxPoint& pt);
	virtual void OnMouseUp(int button, const wxPoint& pt);
	virtual void OnMouseMove(const wxPoint& pt);
	virtual void OnMouseWheel(int wheel, const wxPoint& pt);

	virtual ~BaseState();
};