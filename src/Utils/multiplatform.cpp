#include "multiplatform.h"
#include <iostream>

#if WIN32
	#include <windows.h>
	void MSSleep(int ms) 
	{ 
		Sleep(ms); 
	}
#else
	#include <unistd.h>
	void MSSleep(int ms) 
	{ 
		usleep(ms * 1000); 
	}
#endif

#if IS_RPI
	// It may be the case that GPIO functionality is moved 
	// into its own dedicated utility library in the future.
	// (wleu 06/08/2022)
	#include <wiringPi.h>
#endif

namespace cvg { namespace multiplatform 
{
	static bool initedGPIO = false;

	bool InitGPIO()
	{
		if(initedGPIO)
			return false;

#if IS_RPI
		std::cout << "Initializing RaspberryPi GPIO - via WiringPi" << std::endl;
		wiringPiSetup();
#else
		std::cout << "Ignoring request to initialize GPIO on Windows (not supported on platform)." << std::endl;
		return false;

#endif
		// Finalization for all platforms that support GPIOs
		// - for now only RPi.
		initedGPIO = true;
		return true;
	}
}}