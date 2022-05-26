#pragma once

#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <wx/timer.h>
#include "Utils/cvgOptions.h"
#include "FontMgr.h"

#include <boost/date_time/posix_time/posix_time.hpp>

class MainWin;
class BaseState;

/// <summary>
/// The main OpenGL viewport where all content is rendered.
/// </summary>
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

	/// <summary>
	/// The app is in a draw loop (i.e., redraw once every 30 seconds).
	/// In order to idle for the correct amount of time between loops,
	/// we need to stopwatch how much time application processing
	/// has already taken.
	/// </summary>
	boost::posix_time::ptime lastStopwatch;

	FontWU fontMousePos;
	wxPoint lastDownDbgMouse;

public:
	/// <summary>
	/// The application preferences. This cache should not be
	/// used directly for anything except saving and loading.
	/// 
	/// If the values from the option are used in systems, they 
	/// should be pulled into variables of the actual systems 
	/// they're relevant for.
	/// </summary>
	cvgOptions cachedOptions;

	// The viewport size will need to be set in an initial call
	// to ApplyOptions.
	//
	// Depending on where/how we actually use these values, these
	// variables may be moved into a more relevant location, such
	// as a state.
	int viewportX				= 640;
	int viewportY				= 480;
	int viewportOfsX			= 0;
	int viewportOfsY			= 0;
	int mousepadOffsX			= 0;
	int mousepadOffsY			= 500.0f;
	float mousepadScale			= 0.4f;
	bool fullscreen				= true;
	bool initStaticResources	= false;

protected:
	void _SetupGLDimensions();

public:
	GLWin(MainWin* parent);
	~GLWin();

	inline MainWin* Parent() 
	{return this->typedParent;}

	BaseState* CurrState();

	void SetGLCurrent();

	void DrawGraphics(const wxSize& sz);

	void InitializeOptions();
	void LoadHMDAppOptions();
	void LoadHMDAppOptions(const cvgOptions& opts);

	/// <summary>
	/// Called by MainWin when the GLWin is initialized with a valid
	/// graphics context, and one-time OpenGL 
	/// </summary>
	void InitStaticGraphicResources();

	/// <summary>
	/// This should be called once, at the end of application's life.
	/// For any resources allocated from InitStaticGraphicResources(),
	/// they should be released from here - before the OpenGL context
	/// and the application are destroyed.
	/// </summary>
	void ReleaseStaticGraphicResources();

public:

	//		WX EVENT CALLBACKS
	//
	//////////////////////////////////////////////////

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
	void OnLMouseDoubleDown(wxMouseEvent& evt);
	void OnLMouseUp(wxMouseEvent& evt);
	void OnMMouseDown(wxMouseEvent& evt);
	void OnMMouseDoubleDown(wxMouseEvent& evt);
	void OnMMouseUp(wxMouseEvent& evt);
	void OnRMouseDown(wxMouseEvent& evt);
	void OnRMouseDoubleDown(wxMouseEvent& evt);
	void OnRMouseUp(wxMouseEvent& evt);
	void OnMouseMotion(wxMouseEvent& evt);
	void OnMouseWheel(wxMouseEvent& evt);

public:
	DECLARE_EVENT_TABLE();
};

