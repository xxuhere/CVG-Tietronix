#pragma once
#include <chrono>

/// <summary>
/// A timing utility class to maintain a consistent intervals.
/// 
/// It's designed to profile how long a chunk of processing takes,
/// and calculates how much time the process would need to sleep
/// to loop at specific intervals.
/// 
/// The class also considers rollover of excess time truncated 
/// between timings, to the microsecond level.
/// </summary>
class cvgStopwatchLeft
{
private:
	/// <summary>
	/// The rollover of microseconds, that tries to account for the
	/// fact that we're working with microseconds, but the interface
	/// is dealing with milliseconds. Any built up time that is lost
	/// from this truncation is built over time and reapplied when
	/// cumulatively significant.
	/// </summary>
	long long excessMicroseconds = 0;

	std::chrono::high_resolution_clock::time_point lastTime;

public:

	cvgStopwatchLeft();

	int MSLeft(int msToPass);

	int MSLeft33();
};