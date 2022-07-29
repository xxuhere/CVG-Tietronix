#pragma once
#include <chrono>

/// <summary>
/// A timer utility that checks if discrete chunks of time have passed.
/// 
/// This utility is primarily to make sure we drop and pad video frames
/// when saving to a video stream to make sure video footage is as close
/// to being as real time as possible.
/// </summary>
class cvgGrabTimer
{
private:
	// When polling for milliseconds, we work in the microseconds range
	// as to try not to loose that precision. Any extra microseconds we
	// encounter are accumulated until they can contribute as a millisecond.
	//
	// While we only need 1000 microseconds to be a millisecond, we're
	// overly conservative with the datatype. Arguably too over-conservative.
	long long excessMicroseconds = 0;

	/// <summary>
	/// A recording of how many milliseconds have passed.
	/// </summary>
	long accumMilli = 0;

	/// <summary>
	/// The last time polling occured.
	/// </summary>
	std::chrono::high_resolution_clock::time_point lastTime;

protected:
	/// <summary>
	/// Polls the amount of time that has occured since the last flush, and
	/// update the internal millisecond counter.
	/// 
	/// Flush may be a misnomer, although Update_and_accumulate_MS_time_since_last_update()
	/// might have been too verbose.
	/// </summary>
	void FlushTime();

public:
	cvgGrabTimer();

	/// <summary>
	/// Reset the millisecond counter.
	/// </summary>
	/// <param name="startAccumMS">
	/// The starting internal millisecond counter. If theres no reason to use
	/// any specific value, just set to 0.
	/// </param>
	void Reset(int startAccumMS);

	/// <summary>
	/// Flush the time ellapsed timer and attempt to grab a chunk of available milliseconds.
	/// </summary>
	/// <param name="ms">
	/// The specified number of allocated milliseconds to attempt to remove.
	/// </param>
	/// <returns>
	/// If true, the specified number of milliseconds was removed, and the action tied to that
	/// chunk of time should be performed.
	/// If false, there wasn't enough time ellapsed and accumulated to remove that much time,
	/// and the action tied to the chunk of time should be omitted.
	/// </returns>
	bool GrabMS(int ms);
};