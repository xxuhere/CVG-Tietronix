#pragma once

// Miscellaneous utility functions that are pretty much the same
// on multiple platforms, but may require different 
// pplatform-specific implementations.

/// <summary>
/// Sleep the thread for a specified number of milliseconds.
/// </summary>
void MSSleep(int ms);


// The only effective way known to branch Windows from RPi with
// preprocessor checks is to use __arm__, but really that checks
// for ARM. 
//
// But it's used until we have a better solution. Either way, IS_RPI
// can be used to signal the intention is to preprocessor branch for RPi.
#define IS_RPI __arm__ 

/// <summary>
/// The concept of having the same functionality implemented differently
/// on multiple platform inherently runs the risk of any wrapper utilities
/// we make potentially colliding with API names from anything involved.
/// 
/// To be extra-safe, our stuff should be scoped into its own namespace.
/// </summary>
namespace cvg { namespace multiplatform 
{ 
	// GPIO systems are different depdending on the platform (that 
	// mostly means it's available on the RPi and missing on
	// Windows).
	//
	// But, while used for the laser system, it's forseeable that
	// other things may want to use the GPIO system, and they should
	// all share the same initialization logic.
	bool InitGPIO();
}}
