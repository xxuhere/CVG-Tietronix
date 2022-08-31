#pragma once
#include "../Utils/cvgCoroutine.h"
#include "../MainWin.h"
#include "../GLWin.h"
#include "../CamVideo/SnapRequest.h"
#include <string>
#include <vector>

/// <summary>
/// A coroutine that 
/// - turns the laser off, 
/// - snaps a series of photos,
/// - turns the laser on,
/// - snaps a series of photos,
/// - set the laser back to the default state before the coroutine.
/// </summary>
/// 
class CoroutineSnapWithLasers : public cvgCoroutine
{
private:
	/// <summary>
	/// Controls and access to application data and the laser.
	/// </summary>
	MainWin* mainApp;
	GLWin* glWin;

	bool laserOnAtStart = false;

	int step = 0;

	/// <summary>
	/// The snapshots that we're waiting for the application to perform
	/// before continuing with the next step.
	/// </summary>
	std::vector<SnapRequest::SPtr> queuedSnaps;

	std::string phaseName;
	std::string snapTime;

protected:
	bool ImplStart() override;
	bool ImplEnd() override;
	bool ImplStep() override;
	bool ImplDispose() override;

	bool RequestSnapAll(const std::string& baseName);
	bool AwaitingOnAnySnapRequests();

public:
	CoroutineSnapWithLasers(
		MainWin* app, 
		GLWin* win,
		const std::string& phaseName);
};