#pragma once

// Miscellaneous utility functions that are pretty much the same
// on multiple platforms, but may require different 
// pplatform-specific implementations.

void MSSleep(int ms);
#if  __arm__ 
	#define IS_RPI 1
#endif
