#pragma once

#include <wx/wx.h>
#include <wx/glcanvas.h>

class HMDOpApp;
class GLWin;
class MainWin;

/// <summary>
/// The base state for the application state machine.
/// 
/// These are the various full-screen application states that control
/// all the aspects of what the application can currently do at the moment,
/// - What's rendered to the screen.
/// - What logic is being processed in the background.
/// - How input is being handled.
/// - Rules to transition into other states.
/// </summary>
class BaseState
{
public:
	/// <summary>
	///	An enum listing out all the states that there will be
	/// subclasses for. See BaseState::GetState() for usage.
	/// </summary>
	enum AppState
	{
		/// <summary>
		/// The introduction state. This may have loading screen contents,
		/// as well as the application's introductory mast - with the product
		/// name and organization credits.
		/// </summary>
		Intro,

		/// <summary>
		/// The initialize camera state. This state holds the user until both
		/// cameras have been successfully loaded, so that the main operation
		/// state can start off with two functional cameras and itself doesn't
		/// need to handle uninitialized cameras.
		/// </summary>
		InitCams,

		/// <summary>
		/// 
		/// </summary>
		MainOp
	};

	// NOTE: If we make UISys a dependency of the state system, this
	// struct would probably be obsolete in lieu of UIColor4.
	//
	/// <summary>
	/// A Color represented by 3 float components, RGB.
	/// </summary>
	struct C3F
	{
		float r;	// Red
		float g;	// Green
		float b;	// Blue

		C3F();
		C3F(float r, float g, float b);
	};

private:
	// CACHED APPLICATION VARIABLES
	//
	//////////////////////////////////////////////////
	//
	//
	// These are private, because subclasses don't need 
	// to be resetting these.
	//
	// Note that these pointers are application objects
	// that are expected to outlive the application state
	// machine (and therefore outline all BaseState instances).

	/// <summary>
	/// The cached value of what enum state the BaseState subclass is.
	/// </summary>
	AppState	_state;

	/// <summary>
	/// A cached reference to the application.
	/// </summary>
	HMDOpApp*	_app	= nullptr;

	/// <summary>
	/// A cached reference to the application's graphics rendering viewport window.
	/// </summary>
	GLWin*		_view	= nullptr;

	/// <summary>
	/// A cached reference to the application's toplevel window..
	/// </summary>
	MainWin*	_core	= nullptr;

public:
	BaseState(AppState state, HMDOpApp* app, GLWin* view, MainWin* core);

	inline AppState GetState() {return this->_state;}
	inline HMDOpApp* GetApp() {return this->_app;}
	inline GLWin* GetView() {return this->_view;}
	inline MainWin* GetCoreWindow() {return this->_core;}

public:
	// Note the terminolog used here, an "event" is a callback that's called
	// to notify the object every time a specific event has occured in the application
	// that the object, or any of its subclasses, may need to be aware of.

	/// <summary>
	/// When active, render the state to the OpenGL viewport.
	/// </summary>
	/// <param name="sz">The size of the window.</param>
	virtual void Draw(const wxSize& sz) = 0;

	/// <summary>
	/// When active, run non-rendering code at regular intervals.
	/// </summary>
	/// <param name="dt"></param>
	virtual void Update(double dt) = 0;

	/// <summary>
	/// Event, called when the state is made the active state.
	/// </summary>
	virtual void EnteredActive() = 0;

	/// <summary>
	/// Event, called when the state is no longer the active state.
	/// </summary>
	virtual void ExitedActive() = 0;

	/// <summary>
	/// Event, called during the initialization phase of the application.
	/// 
	/// This will only be called once, and should be used to gather or
	/// generate one-time resources.
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// Event, called before the application closes.
	/// 
	/// This will only be called once, and should mainly be used 
	/// to release any resources obtained from Initialize().
	/// </summary>
	virtual void ClosingApp() = 0;

	/// <summary>
	/// Query the prefferred background color for the state. This 
	/// will usually be black, as these states are meant to be 
	/// part of an HMD app that will be hooked into a pass-through
	/// display where black is transparent.
	/// </summary>
	/// <returns></returns>
	virtual C3F BackgroundColor();

	/// <summary>
	/// Event, called when a keyboard key is pressed when the 
	/// application has keyboard focus.
	/// </summary>
	/// <param name="key">
	/// The key that was pressed. This directly uses wxWidgets key representations.
	/// </param>
	virtual void OnKeydown(wxKeyCode key);

	/// <summary>
	/// Event, called when a keyboard key is released.
	/// </summary>
	/// <param name="key">
	/// The key that was pressed. This directly uses wxWidgets key representations.
	/// </param>
	virtual void OnKeyup(wxKeyCode key);

	/// <summary>
	/// Event, called when a mouse button is pressed.
	/// </summary>
	/// <param name="button">
	/// The button that was pressed:
	/// 0 : The left mouse button
	/// 1 : The middle mouse button (wheel)
	/// 2 : The right mouse button
	/// </param>
	/// <param name="pt">The screen (pixel) location of where the mouse was at the time.</param>
	virtual void OnMouseDown(int button, const wxPoint& pt);

	/// <summary>
	/// Event, called when a previous mouse button is released.
	/// </summary>
	/// <param name="button">
	/// The button that was released:
	/// 0 : The left mouse button
	/// 1 : The middle mouse button (wheel)
	/// 2 : The right mouse button
	/// </param>
	/// <param name="pt">The screen (pixel) location of where the mouse was at the time.</param>
	virtual void OnMouseUp(int button, const wxPoint& pt);

	/// <summary>
	/// Event, called when the mouse is moved, when on top of the application.
	/// </summary>
	/// <param name="pt">The screen (pixel) location of where the mouse was at the time.</param>
	virtual void OnMouseMove(const wxPoint& pt);

	/// <summary>
	/// Event, called when the mouse wheel is rolled.
	/// </summary>
	/// <param name="wheel">The wheel rotation amount.</param>
	/// <param name="pt">The screen (pixel) location of where the mouse was at the time.</param>
	virtual void OnMouseWheel(int wheel, const wxPoint& pt);

	virtual ~BaseState();
};