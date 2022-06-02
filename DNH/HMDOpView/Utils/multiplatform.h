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
