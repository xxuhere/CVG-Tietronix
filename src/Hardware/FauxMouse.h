#pragma once

#include "IHardware.h"
#include "../Utils/multiplatform.h"
#include "../Utils/cvgAssert.h"
#include <thread>
#include <wx/wx.h>

#if IS_RPI
	#include <wiringPi.h>
#endif

// GPIO Pins used as inputs to emulation the mouse.
#define WPI_PIN_LEFT	27
#define WPI_PIN_MID		28
#define WPI_PIN_RIGHT	29

/// <summary>
/// A background equipment process that will emulate the mouse.
/// 
/// This will allow developers to test and use the application on
/// a desktop environment, but then swap out with GPIO pin hardware if
/// desired, without requiring any extra changes.
/// </summary>
class FauxMouse : public IHardware
{
protected:

	/// <summary>
	/// A button tracking state for an individual emulated mouse.
	/// </summary>
	struct ButtonState
	{
		/// <summary>
		/// The wiringPi GPIO pin to query.
		/// </summary>
		int pin;

		/// <summary>
		/// Tracking if the button is pressed or released.
		/// </summary>
		bool down = false;

		/// <summary>
		/// The wxWidgets (mouse) event type for when teh button is pressed.
		/// </summary>
		wxEventType eventDown;

		/// <summary>
		/// The wxWidgets (mouse) event type for when the button is released.
		/// </summary>
		wxEventType eventUp;
	};

	/// <summary>
	/// The event handler to inject mouse events into. This is expected to
	/// be the OpenGL window.
	/// </summary>
	wxEvtHandler* evtSink = nullptr;

	/// <summary>
	/// The 3 mouse buttons.
	/// </summary>
	ButtonState buttonStates[3];

	/// <summary>
	/// Is the thread currently running?
	/// </summary>
	bool threadRunning = false;

	/// <summary>
	/// Should the thread keep running? This is an accessible flag that can
	/// be set to false while the thread is running, to signal it to shut down.
	/// </summary>
	bool keepRunning = false;

	/// <summary>
	/// Has the thread formally ended? This can be checked when shutting down
	/// a thread to see if it has successfully ended. 
	/// 
	/// This may be excessive and can probably be replaced with threadRunning.
	/// </summary>
	bool hasShutdown = false;

	/// <summary>
	/// Dynamically allocated thread.
	/// </summary>
	std::thread* pollThread = nullptr;

public:

	/// <summary>
	/// Thread polling function. It polls the GPIO and checks if simulated
	/// mouse messages should be sent.
	/// </summary>
	static void ThreadPollFn(FauxMouse* _this);

public:
	// This class is expected to be instanciated in MainWin.cpp - 
	// it's a singleton, with a max instance count of 1.
	FauxMouse(wxEvtHandler* evtSink);

	std::string HWName() const override;
	bool Initialize() override;
	bool Validate() override;
	bool Shutdown() override;
};