#include "cvgGrabTimer.h"

cvgGrabTimer::cvgGrabTimer()
{
	this->Reset(0);
}

void cvgGrabTimer::Reset(int startAccumMS)
{
	this->excessMicroseconds = startAccumMS;
	this->lastTime = std::chrono::high_resolution_clock::now();
}

void cvgGrabTimer::FlushTime()
{
	auto origOldTime = this->lastTime;
	this->lastTime = std::chrono::high_resolution_clock::now();

	auto ellapsed = this->lastTime - origOldTime;
	long long microseconds = 
		std::chrono::duration_cast<std::chrono::microseconds>(ellapsed).count();

	// The rest of this calculation will take up time, but it's going to be
	// accounted for, on the next TimeLeft.
	int msPassed = (int)(microseconds / 1000);
	this->excessMicroseconds += microseconds % 1000;

	// If excess microseconds build up past a second, account for it.
	if(this->excessMicroseconds)
	{ 
		msPassed += this->excessMicroseconds / 1000;
		this->excessMicroseconds %= 1000;
	}

	this->accumMilli += msPassed;
}

bool cvgGrabTimer::GrabMS(int ms)
{
	this->FlushTime();
	if(this->accumMilli >= ms)
	{
		this->accumMilli -= ms;
		return true;
	}
	return false;
}