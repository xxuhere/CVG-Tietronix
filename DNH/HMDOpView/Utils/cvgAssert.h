#pragma once
#include <cassert>
#include <iostream>

#if _DEBUG
	#define cvgAssert(a, msg) \
	if (!(a)) {\
		std::cout<<"ASSERT! "<<msg<<std::endl;\
		assert(a);\
	}
#else
	#define cvgAssert(a, msg) 
#endif
//This will be our assert
