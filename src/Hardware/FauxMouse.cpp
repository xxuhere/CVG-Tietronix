#include "FauxMouse.h"

// All this stuff should really only be available for RPi, and not
// Windows/Visual Studio, but we add it and expose some stuff just
// to make development in the target development environment (WinOS)
// easier.

FauxMouse::FauxMouse(wxEvtHandler* evtSink)
{

#if !IS_RPI
	cvgAssert(false, "FauxMouse only intended for RPi.");
#else
	this->evtSink = evtSink;
	cvgAssert(this->evtSink != nullptr, "Using the FauxMouse without a sink, disable its use if not using it.");
#endif
}

void FauxMouse::ThreadPollFn(FauxMouse* _this)
{
	// How many times to poll a second for simulated
	// mouse messages.
	//
	// It's arguable how high this should be, but really 
	// anything above the expected FPS (30?) is gratuitous.
	const int PollsPerSecond = 100;

#if IS_RPI
	while(_this->keepRunning)
	{
		for(ButtonState& bus : _this->buttonStates)
		{
			bool buttonDown = digitalRead(bus.pin);

			if(bus.down)
			{
				if(!buttonDown) // Mouse button up?
				{
					bus.down = false;

					_this->evtSink->QueueEvent(
						new wxMouseEvent(bus.eventUp));

					// Without this, the application will take forever to
					// process the QueuedEvent - and while there are other
					// way to have the event handler process the event faster,
					// they're not thread safe and they're finicky. It's unknown
					// how thread-safe ProcessPendingEvents() is, but it seems
					// to work as expected at the moment.
					_this->evtSink->ProcessPendingEvents();
					
				}
			}
			else
			{
				if(read) // Mouse button down?
				{
					bus.down = true;

					_this->evtSink->QueueEvent(
						new wxMouseEvent(bus.eventDown));

					_this->evtSink->ProcessPendingEvents();
				}
			}
		}


		MSSleep(1000/PollsPerSecond);
	}
#endif
	_this->hasShutdown = true;
}

std::string FauxMouse::HWName() const
{
	return "FauxMouse";
}

bool FauxMouse::Initialize()
{
#if IS_RPI
	cvg::multiplatform::InitGPIO();

	this->buttonStates[0].pin		= WPI_PIN_LEFT;
	this->buttonStates[0].eventDown = wxEVT_LEFT_DOWN;
	this->buttonStates[0].eventUp	= wxEVT_LEFT_UP;
	//
	this->buttonStates[1].pin		= WPI_PIN_MID;
	this->buttonStates[1].eventDown = wxEVT_MIDDLE_DOWN;
	this->buttonStates[1].eventUp	= wxEVT_MIDDLE_UP;
	//
	this->buttonStates[2].pin		= WPI_PIN_RIGHT;
	this->buttonStates[2].eventDown = wxEVT_RIGHT_DOWN;
	this->buttonStates[2].eventUp	= wxEVT_RIGHT_UP;

	for(ButtonState& bus : this->buttonStates)
	{
		pinMode (bus.pin, INPUT);
	}

	this->pollThread = 
		new std::thread(
			[this]
			{
				this->keepRunning = true;
				this->threadRunning = true;
				ThreadPollFn(this);
				this->threadRunning = false;
			});

	std::cout << "Using FauxMouse" << std::endl;
#endif

	// We may want to move initialization code from the
	// constructor to here.
	return true;
}

bool FauxMouse::Validate()
{
	return true;
}

bool FauxMouse::Shutdown()
{
	if(this->threadRunning == false)
		return true;

#if IS_RPI 
	cvgAssert(this->pollThread != nullptr, "Active FauxMouse, but its polling thread is null.");

	this->keepRunning = false;
	while(this->threadRunning)
	{
		MSSleep(10);
	}
	this->pollThread->join();
	delete this->pollThread;
	this->pollThread = nullptr;

	return true;
#else
	cvgAssert(true, "FauxMouse unexpectedly active on non-RPi platform.");
	return false;
#endif
}