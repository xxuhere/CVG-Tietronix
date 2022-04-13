#include "multiplatform.h"

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
		usleep(ms); 
	}
#endif