#pragma once

#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <wx/timer.h>

class MainWin;
class BaseState;

class GLWin : public wxGLCanvas
{
public:
	enum class CMDID
	{
		// The ID for the app redraw
		RedrawTimer
	};

	// For now it's just used as it's set for the entire
	// duration of the application, but we may make the
	// target FPS modifyable in the future.
	int targetFramerate = 60;

private:
	MainWin* typedParent;
	wxGLContext* ctx = nullptr;
	wxTimer redrawTimer;


public:
	GLWin(MainWin* parent);
	~GLWin();

	inline MainWin* Parent() 
	{return this->typedParent;}

	BaseState* CurrState();

	void DrawGraphics(const wxSize& sz);

	void OnResize(wxSizeEvent& evt);
	void OnPaint(wxPaintEvent& evt);
	void OnClose(wxCloseEvent& evt);
	void OnRedrawTimer(wxTimerEvent& evt);

	// Event handlers
	// these all are simple event handlers that delegate 
	// the input to the current active State.
	void OnKeyDown(wxKeyEvent& evt);
	void OnKeyUp(wxKeyEvent& evt);
	void OnLMouseDown(wxMouseEvent& evt);
	void OnLMouseUp(wxMouseEvent& evt);
	void OnMMouseDown(wxMouseEvent& evt);
	void OnMMouseUp(wxMouseEvent& evt);
	void OnRMouseDown(wxMouseEvent& evt);
	void OnRMouseUp(wxMouseEvent& evt);
	void OnMouseMotion(wxMouseEvent& evt);
	void OnMouseWheel(wxMouseEvent& evt);

public:
	DECLARE_EVENT_TABLE();
};

