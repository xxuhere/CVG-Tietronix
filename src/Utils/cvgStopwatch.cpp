#include "cvgStopwatch.h"

cvgStopwatch::cvgStopwatch()
{
	this->Restart();
}

float cvgStopwatch::Seconds(bool restart)
{
	return this->Milliseconds(restart) / 1000.0f;
}

int cvgStopwatch::Milliseconds(bool restart)
{
	std::chrono::high_resolution_clock::time_point now = 
		std::chrono::high_resolution_clock::now();

	long ret = 
		(long)std::chrono::duration_cast<std::chrono::milliseconds>(
			now - this->lastTime).count();

	if(restart)
		this->lastTime = now;

	return ret;
}

long long cvgStopwatch::Microseconds(bool restart)
{
	std::chrono::high_resolution_clock::time_point now = 
		std::chrono::high_resolution_clock::now();

	long long ret = std::chrono::duration_cast<std::chrono::microseconds>(now - this->lastTime).count();

	if(restart)
		this->lastTime = now;

	return ret;
}

long long cvgStopwatch::Nanoseconds(bool restart)
{
	std::chrono::high_resolution_clock::time_point now = 
		std::chrono::high_resolution_clock::now();

	long long ret = std::chrono::duration_cast<std::chrono::nanoseconds>(now - this->lastTime).count();

	if(restart)
		this->lastTime = now;

	return ret;
}

void cvgStopwatch::Restart()
{
	this->lastTime = std::chrono::high_resolution_clock::now();
}