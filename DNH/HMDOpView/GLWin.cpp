#include "GLWin.h"
#include "MainWin.h"
#include <iostream>
#include "CamVideo/CamStreamMgr.h"
#include "UISys/UISys.h"
#include "HMDOpApp.h"

#include "LoadAnim.h"

wxBEGIN_EVENT_TABLE(GLWin, wxGLCanvas)
	EVT_SIZE		(GLWin::OnResize)
	EVT_PAINT		(GLWin::OnPaint)
	EVT_CLOSE		(GLWin::OnClose)
	EVT_TIMER		((int)CMDID::RedrawTimer, GLWin::OnRedrawTimer)

	EVT_KEY_DOWN		( GLWin::OnKeyDown		)
	EVT_KEY_UP			( GLWin::OnKeyUp		)
	EVT_LEFT_DOWN		( GLWin::OnLMouseDown	)
	EVT_LEFT_DCLICK		( GLWin::OnLMouseDown	)
	EVT_LEFT_UP			( GLWin::OnLMouseUp		)
	EVT_MIDDLE_DOWN		( GLWin::OnMMouseDown	)
	EVT_MIDDLE_DCLICK	( GLWin::OnMMouseDown	)
	EVT_MIDDLE_UP		( GLWin::OnMMouseUp		)
	EVT_RIGHT_DOWN		( GLWin::OnRMouseDown	)
	EVT_RIGHT_DCLICK	( GLWin::OnRMouseDown	)
	EVT_RIGHT_UP		( GLWin::OnRMouseUp		)
	EVT_MOTION			( GLWin::OnMouseMotion	)
	EVT_MOUSEWHEEL		( GLWin::OnMouseWheel	)
wxEND_EVENT_TABLE()


GLWin::GLWin(MainWin* parent)
	:	wxGLCanvas(parent, wxID_ANY),
		redrawTimer(this, (int)CMDID::RedrawTimer),
		cachedOptions(2)
{
	this->typedParent = parent;

	wxGLContextAttrs attrs;
	attrs.MajorVersion(2).EndList();

	this->ctx = new wxGLContext(this, nullptr, &attrs);

	this->Show();
	this->SetCurrent(*this->ctx);

	this->SetBackgroundStyle(wxBG_STYLE_PAINT);

	int expectedMS = (int)(1000.0 / (double)targetFramerate);
	this->lastStopwatch = boost::posix_time::microsec_clock::local_time();
	this->redrawTimer.Start(expectedMS, false);
}

GLWin::~GLWin()
{
	delete ctx;
	this->ctx = nullptr;
}

void GLWin::_SetupGLDimensions()
{
	this->SetCurrent(*this->ctx);

	wxSize size = this->GetClientSize();

	glViewport(0, 0, size.x, size.y);
	std::cout << "Changing GL viewport to " << size.x << ", " << size.y << std::endl;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, size.x, size.y, 0.0, 0.0f, 1.0f);
	std::cout << "Changing Ortho view to " << size.x << ", " << size.y << std::endl;

	glMatrixMode(GL_MODELVIEW);
}

BaseState* GLWin::CurrState()
{
	return this->Parent()->CurrState();
}

void GLWin::SetGLCurrent()
{
	this->SetCurrent(*this->ctx);
}

void GLWin::DrawGraphics(const wxSize& sz)
{
	if(this->ctx == nullptr)
		return;

	this->SetCurrent(*this->ctx);

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	BaseState* cur = this->CurrState();
	if(cur != nullptr)
	{ 
		BaseState::C3F bgCol = cur->BackgroundColor();
		glClearColor( bgCol.r, bgCol.g, bgCol.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		cur->Draw(sz);
	}
	else
	{
		glClearColor( 0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	}
}

void GLWin::InitializeOptions()
{
	const char* szConfigLocation = wxGetApp().appOptionsLoc.c_str();

	std::cout << "Initializing from options file " << szConfigLocation << std::endl;
	if(!this->cachedOptions.LoadFromFile(szConfigLocation))
	{
		// If there's no configuration file, make one. Chances
		// are this->cachedOptions will be the default object,
		// but either way if we save whatever is inside of it,
		// the file and our options will match.
		std::cout << "Creating default options file at " << szConfigLocation << std::endl;
		this->SaveOptions(szConfigLocation);
	}
	this->LoadHMDAppOptions();
	std::cout << "Finished loading." << std::endl;
}

void GLWin::LoadHMDAppOptions()
{
	this->LoadHMDAppOptions(this->cachedOptions);
}

void GLWin::LoadHMDAppOptions(const cvgOptions& opts)
{
	this->viewportX		= opts.viewportX;
	this->viewportY		= opts.viewportY;
	this->viewportOfsX	= opts.viewportOffsX;
	this->viewportOfsY	= opts.viewportOffsY;
	//
	this->mousepadOffsX	= opts.mousepadX;
	this->mousepadOffsY	= opts.mousepadY;
	this->mousepadScale = opts.mousepadScale;
	//
	UISys::ToggleDebugView(opts.drawUIDebug);
	//
	this->fullscreen	= opts.fullscreen;
}

void GLWin::SaveOptions(const std::string& saveFilepath) const
{
	this->cachedOptions.SaveToFile(saveFilepath);
}

void GLWin::SaveOptions()
{
	this->SaveOptions(wxGetApp().appOptionsLoc);
}

void GLWin::OnResize(wxSizeEvent& evt)
{
	if(this->ctx == nullptr || !this->initStaticResources)
		return;

	this->_SetupGLDimensions();
	this->Refresh(false);
}

void GLWin::OnPaint(wxPaintEvent& evt)
{
	// Not used, but its construction and destruction within the
	// function may be needed for a proper OnPaint handler.
	wxPaintDC dc(this);

	this->SetCurrent(*this->ctx);

	if(!this->initStaticResources)
	{ 

		this->InitStaticGraphicResources();
		this->fontMousePos = FontMgr::GetInstance().GetFont(10);

		// If we've just initialized, then we've never successfully 
		// handled a resize and updated the viewport state yet.
		this->_SetupGLDimensions();
		this->typedParent->InitializeAppStateMachine();
	}

	glColor3f(1.0f, 1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	wxSize sz = this->GetSize();
	this->DrawGraphics(sz);

	if(UISys::IsDebugView() && this->fontMousePos.IsValid())
	{
		glColor3f(1.0f, 0.0f, 1.0f);
		// Show ellapsed application time
		std::stringstream sstrmEll;
		sstrmEll << std::fixed << std::setprecision(2);
		sstrmEll << "Seconds: " << this->appEllapsedTimer.Milliseconds(false) / 1000.0f;
		this->fontMousePos.RenderFont(sstrmEll.str().c_str(), sz.x - 200, sz.y - 75);

		// Show mouse position.
		std::stringstream sstrmDbgMouse;
		sstrmDbgMouse << "x: " << this->lastDownDbgMouse.x << " - y: " << this->lastDownDbgMouse.y;
		this->fontMousePos.RenderFont(sstrmDbgMouse.str().c_str(), sz.x - 200, sz.y - 50);

		// Show rendered screen resolution.
		std::stringstream sstrmDbgRes;
		sstrmDbgRes << "Res W: " << sz.x << " - H: " << sz.y;
		this->fontMousePos.RenderFont(sstrmDbgRes.str().c_str(), sz.x - 200, sz.y - 25);
	}

	this->SwapBuffers();
}


void GLWin::OnClose(wxCloseEvent& evt)
{
	if(this->ctx != nullptr)
	{ 
		delete this->ctx;
		this->ctx = nullptr;
	}
	evt.Skip();
}

void GLWin::OnRedrawTimer(wxTimerEvent& evt)
{
	this->Refresh(false);

	// Find the time since the last update, the delta time,
	// which some Update() functions may need for animations
	// and other realtime things.
	boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
	boost::posix_time::time_duration diff = now - this->lastStopwatch;
	double deltaTimeSecs = (double)diff.total_microseconds() / 1000000.0;
	this->lastStopwatch = now;

	BaseState* cur = this->Parent()->CurrState();
	if(cur != nullptr)
		cur->Update(deltaTimeSecs);

	// Any one individual state shouldn't be left in charge of cleaning
	// the snaps, so its done at the outer level.
	this->typedParent->PerformMaintenenceCycle();
}

void GLWin::InitStaticGraphicResources()
{
	std::cout << "Entering InitStaticGraphicResources" << std::endl;

	if(this->initStaticResources)
		return;

	this->initStaticResources = true;

	LoadAnim::LoadRet loadAnimSuc = LoadAnim::EnsureInit();
	if(loadAnimSuc == LoadAnim::LoadRet::Error)
	{
		wxMessageBox(
			"Could not load assets.", 
			"Could not load assets for load screen. Make sure Load_*.png files are where expected.");
	}

	std::cout << "Exiting InitStaticGraphicResources" << std::endl;
}

void GLWin::ReleaseStaticGraphicResources()
{
	LoadAnim::Uninit();
}

// All the state delegation functions have the same boilerplate
// checks for the current state.
#define GET_CURR_STATE_OR_RETURN(varname) \
	BaseState * varname = this->CurrState(); \
	if(varname == nullptr ){ return; }

void GLWin::OnKeyDown(wxKeyEvent& evt)
{
	if(evt.GetKeyCode() == WXK_INSERT)
	{
		// Reload options during runtime.
		this->InitializeOptions();
	}
	else if(evt.GetKeyCode() == WXK_PAUSE)
	{
		UISys::ToggleDebugView();
	}

	GET_CURR_STATE_OR_RETURN(cur);
	cur->OnKeydown((wxKeyCode)evt.GetKeyCode());
}

void GLWin::OnKeyUp(wxKeyEvent& evt)
{
	GET_CURR_STATE_OR_RETURN(cur);
	cur->OnKeyup((wxKeyCode)evt.GetKeyCode());
}

void GLWin::OnLMouseDown(wxMouseEvent& evt)
{
	this->lastDownDbgMouse = evt.GetPosition();

	GET_CURR_STATE_OR_RETURN(cur);
	cur->OnMouseDown(0, evt.GetPosition());
}

void GLWin::OnLMouseDoubleDown(wxMouseEvent& evt)
{
	this->lastDownDbgMouse = evt.GetPosition();

	GET_CURR_STATE_OR_RETURN(cur);
	// States currently don't have a double click, but it still needs
	// to be handled, so we consider it a normal click.
	cur->OnMouseDown(0, evt.GetPosition());
}

void GLWin::OnLMouseUp(wxMouseEvent& evt)
{
	this->lastDownDbgMouse = evt.GetPosition();

	GET_CURR_STATE_OR_RETURN(cur);
	cur->OnMouseUp(0, evt.GetPosition());
}

void GLWin::OnMMouseDown(wxMouseEvent& evt)
{
	this->lastDownDbgMouse = evt.GetPosition();

	GET_CURR_STATE_OR_RETURN(cur);
	cur->OnMouseDown(1, evt.GetPosition());
}

void GLWin::OnMMouseDoubleDown(wxMouseEvent& evt)
{
	this->lastDownDbgMouse = evt.GetPosition();

	GET_CURR_STATE_OR_RETURN(cur);
	cur->OnMouseDown(1, evt.GetPosition());
}

void GLWin::OnMMouseUp(wxMouseEvent& evt)
{
	this->lastDownDbgMouse = evt.GetPosition();

	GET_CURR_STATE_OR_RETURN(cur);
	cur->OnMouseUp(1, evt.GetPosition());
}

void GLWin::OnRMouseDown(wxMouseEvent& evt)
{
	this->lastDownDbgMouse = evt.GetPosition();

	GET_CURR_STATE_OR_RETURN(cur);
	cur->OnMouseDown(2, evt.GetPosition());
}

void GLWin::OnRMouseDoubleDown(wxMouseEvent& evt)
{
	this->lastDownDbgMouse = evt.GetPosition();

	GET_CURR_STATE_OR_RETURN(cur);
	cur->OnMouseDown(2, evt.GetPosition());
}

void GLWin::OnRMouseUp(wxMouseEvent& evt)
{
	this->lastDownDbgMouse = evt.GetPosition();

	GET_CURR_STATE_OR_RETURN(cur);
	cur->OnMouseUp(2, evt.GetPosition());
}

void GLWin::OnMouseMotion(wxMouseEvent& evt)
{
	this->lastDownDbgMouse = evt.GetPosition();

	GET_CURR_STATE_OR_RETURN(cur);
	cur->OnMouseMove(evt.GetPosition());
}

void GLWin::OnMouseWheel(wxMouseEvent& evt)
{
	GET_CURR_STATE_OR_RETURN(cur);
	cur->OnMouseWheel(evt.GetWheelRotation(), evt.GetPosition());
}

#undef GET_CURR_STATE_OR_RETURN