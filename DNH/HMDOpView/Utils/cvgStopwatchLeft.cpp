#include "cvgStopwatchLeft.h"


cvgStopwatchLeft::cvgStopwatchLeft()
{
	this->lastTime = std::chrono::high_resolution_clock::now();
}

int cvgStopwatchLeft::MSLeft(int msToPass)
{
	auto origOldTime = this->lastTime;
	this->lastTime = std::chrono::high_resolution_clock::now();

	auto ellapsed = this->lastTime - origOldTime;
	long long microseconds = 
		std::chrono::duration_cast<std::chrono::microseconds>(ellapsed).count();

	// The rest of this calculation will take up time, but it's going to be
	// accounted for, on the next TimeLeft.
	int msPassed = microseconds / 1000;
	this->excessMicroseconds += microseconds % 1000;

	// If excess microseconds build up past a second, account for it.
	if(this->excessMicroseconds)
	{ 
		msPassed += this->excessMicroseconds / 1000;
		msPassed %= 1000;
	}

	if(msPassed > msToPass)
	{
		this->excessMicroseconds = 0;
		return 0;
	}
	return msToPass - msPassed;
}

// As close as we can get to 30FPS with milliseconds (30 * 33 == 990)
int cvgStopwatchLeft::MSLeft33()
{
	return this->MSLeft(33);
}