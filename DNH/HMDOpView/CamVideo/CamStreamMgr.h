#pragma once

#include "ManagedCam.h"

/// <summary>
/// Manages camera streaming. Note that this is expected to
/// happen in its own thread.
/// 
/// CamStreamMgr::GetInstance().BootConnectionToCamera() should
/// be called once in the app to initialize the main processing 
/// thread.
/// 
/// Shutdown() should be called once, when the application is
/// ready to shutdown and take the CamStreamMgr thread with it.
/// </summary>
class CamStreamMgr
{
private:
	/// <summary>
	/// Singleton instance.
	/// </summary>
	static CamStreamMgr _inst;

public:
	/// <summary>
	/// Public accessor to singleton instance.
	/// </summary>
	static CamStreamMgr& GetInstance();

	/// <summary>
	/// This should be called at the end of the application's
	/// lifetime to properly shutdown the singleton instance.
	/// </summary>
	/// <returns>Success value. This can be ignored.</returns>
	static bool ShutdownMgr();

private:
	
	/// <summary>
	/// The cameras managed in the system.
	/// </summary>
	std::vector<ManagedCam*> cams;

	/// <summary>
	/// Ensure nothing is attempting to modify the cameras
	/// and read from them at the same time.
	/// </summary>
	std::mutex camAccess;

public:
	~CamStreamMgr();

	/// <summary>
	/// Startup the working thread.
	/// </summary>
	bool BootConnectionToCamera(
		int camCt, 
		ManagedCam::PollType pt = ManagedCam::PollType::Deactivated);

	/// <summary>
	/// Get access to the shared pointer of the last polled image.
	/// 
	/// This function will use imageAccess for thread saftey.
	/// </summary>
	/// <param name="idx">The camera index to query.</param>
	/// <returns>
	/// The last polled image. This pointer can be null if an image
	/// has not been polled yet.
	/// </returns>
	cv::Ptr<cv::Mat> GetCurrentFrame(int idx);

	/// <summary>
	/// Get the current frame ID for a camera. The frame ID is a counter
	/// assigned to each frame, which allows checking if current frame
	/// cached has changed.
	/// </summary>
	/// <param name="idx">The camera index to query.</param>
	/// <returns></returns>
	long long GetCameraFeedChanges(int idx);

	/// <summary>
	/// Shutdown the camera manager.
	/// 
	/// This should be called only once, at the end of the app's
	/// lifetime.
	/// </summary>
	/// <returns>True, if successful.</returns>
	bool Shutdown();

	/// <summary>
	/// Get the current running state of a camera thread.
	/// </summary>
	/// <param name="idx">The camera index to query.</param>
	/// <returns></returns>
	ManagedCam::State GetState(int idx);

	/// <summary>
	/// Specify how a camera should be polling for its image stream.
	/// </summary>
	/// <param name="idx">The camera index to query.</param>
	/// <param name="pty">The polling method to use.</param>
	void SetPollType(int idx, ManagedCam::PollType pty);

	/// <summary>
	/// Clear all snapshot request for a specific camera.
	/// </summary>
	/// <param name="idx">The camera index.</param>
	void ClearSnapshotRequests(int idx);

	/// <summary>
	/// Clear all snapshot requests of all cameras.
	/// </summary>
	void ClearAllSnapshotRequests();

	/// <summary>
	/// Queue a snapshot request for the new camera frame.
	/// </summary>
	/// <param name="idx">The camera index to queue.</param>
	/// <returns>The return value</returns>
	SnapRequest::SPtr RequestSnapshot(int idx, const std::string& filename);

private:
	// Only the singleton systems should be in charge of its construction.
	CamStreamMgr();
};