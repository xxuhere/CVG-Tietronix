#pragma once
#include <chrono>

/// <summary>
/// A timing utility for stopwatch functionality.
/// </summary>
class cvgStopwatch
{
private:
	std::chrono::high_resolution_clock::time_point lastTime;

public:
	cvgStopwatch();

	/// <summary>
	/// Track the progress of the stopwatch in milliseconds.
	/// 
	/// The precision of this timer depends on the platform
	/// the code is being run on.
	/// </summary>
	/// <param name="restart">If true, restart the stopwatch.</param>
	/// <returns>
	/// The number of milliseconds since when the stopwatch was started
	/// and when the function was called.
	/// </returns>
	int Milliseconds(bool restart = true);

	/// <summary>
	/// Track the progress of the stopwatch in nicroseconds.
	/// 
	/// The precision of this timer depends on the platform
	/// the code is being run on.
	/// </summary>
	/// <param name="restart">If true, restart the stopwatch.</param>
	/// <returns>
	/// The number of nicroseconds since when the stopwatch was started
	/// and when the function was called.
	/// </returns>
	long long Microseconds(bool reset = true);

	/// <summary>
	/// Track the progress of the stopwatch in nanoseconds.
	/// 
	/// The precision of this timer depends on the platform
	/// the code is being run on.
	/// </summary>
	/// <param name="restart">If true, restart the stopwatch.</param>
	/// <returns>
	/// The number of nanoseconds since when the stopwatch was started
	/// and when the function was called.
	/// </returns>
	long long Nanoseconds(bool restart = true);

	/// <summary>
	/// Restart the stopwatch timer.
	/// </summary>
	void Restart();
};