#pragma once
#include <cassert>
#include <iostream>

#if _DEBUG
	// A custom extention to the assert macro that allows adding
	// in a custom message to the terminal when an assert is detected.
	#define cvgAssert(a, msg) \
	if (!(a)) {\
		std::cerr<<"ASSERT! "<<msg<<std::endl;\
		assert(a);\
	}
#else
	#define cvgAssert(a, msg) 
#endif
//This will be our assert
