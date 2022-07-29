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

	/// <summary>
	/// Check how many more milliseconds need to pass (be slept for)
	/// for the time that's passed so far, plus the return value, to
	/// cause the specified (msToPass) number of milliseconds to be
	/// ellapsed.
	/// 
	/// Note that calling this function will reset the timer.
	/// </summary>
	/// <param name="msToPass">
	/// The number of millisecond to attempt to ellapse.
	/// </param>
	/// <returns>
	/// The number of milliseconds that need to pass for the combined
	/// time of how long since the last time MSLeft was called plus the
	/// return value for msToPass milliseconds to occur. 
	/// 
	/// Note that if time underruns, 0 will be passed as there would be 
	/// nothing we could do with negative time.
	/// </returns>
	int MSLeft(int msToPass);

	/// <summary>
	/// Equivalent to MSLeft(33).
	/// This is a number that will often be checked for, as a common
	/// target for realtime updates will be 30 frames a second, and
	/// the 1000 ms in a second needs to be ~33 to hit as close to
	/// 30FPS with millisecond resolution.
	/// </summary>
	/// <returns>
	/// The equivalent of what MSLeft(33) would return.
	/// </returns>
	int MSLeft33();
};