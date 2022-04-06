#pragma once

#include "CamStreamMgr.h"
#include "SnapRequest.h"

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

#include <mutex>
#include <thread>
#include <memory>
#include <vector>


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
public:

	/// <summary>
	/// The various running states of the manager.
	/// </summary>
	enum class State
	{
		/// <summary>
		/// The manager has not been started yet.
		/// </summary>
		Unknown,

		/// <summary>
		/// The manager's thread is running, but not idling.
		/// </summary>
		Idling,

		/// <summary>
		/// The manager is connecting to the video streaming source.
		/// </summary>
		Connecting,

		/// <summary>
		/// The manager is actively polling video frames from the
		/// streaming source.
		/// </summary>
		Polling,

		/// <summary>
		/// The manager has been permanently shut down.
		/// </summary>
		Shutdown
	};

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
	// OpenCV streaming class.
	cv::VideoCapture stream;

	/// <summary>
	/// Mutex to guard against multiple thread simultaneously
	/// working with curCamFrame.
	/// </summary>
	std::mutex imageAccess;

	/// <summary>
	/// The requests for saving the next saved image. 
	/// 
	/// While chances are there will only be a single image request
	/// per image, we'll be conservative and handle the possibility
	/// of a small unique batch.
	/// </summary>
	std::vector<SnapRequest::SPtr> snapReqs;

	/// <summary>
	/// Mutex to guard single thread access to the snap requests.
	/// </summary>
	std::mutex snapReqsAccess;

	/// <summary>
	/// Shared pointer of the most recent video frame.
	/// </summary>
	cv::Ptr<cv::Mat> curCamFrame;

	/// <summary>
	/// Has there been a request to shut down the manager?
	/// </summary>
	bool _sentShutdown = false;

	/// <summary>
	/// Has the manager been fully shut down?
	/// </summary>
	bool _isShutdown = false;

	/// <summary>
	/// Is the manager expected to be streaming?
	/// </summary>
	bool _shouldStreamBeActive = false;

	/// <summary>
	/// Is the manger streaming?
	/// </summary>
	bool _isStreamActive = false;


	/// <summary>
	/// The working thread for polling and other CamStreamMgr
	/// tasks. This will only be non-null if 
	/// </summary>
	std::thread* camStreamThread = nullptr;

public:
	/// <summary>
	/// When testing mode is turned on, camera streaming will be replaced with
	/// showing a canned image. This allows us to iterate faster by not 
	/// requiring the camera video feed to fully initialize before starting up.
	/// </summary>
	bool testingMode = false;

	/// <summary>
	/// This should match with the number of times curCamFrame has been updated,
	/// so we if need track of the last time we polled curCamFrame and the 
	/// change counter, we know if we can expect a change if we poll again.
	/// 
	/// For simplicity, and because there's little threading danger doing so,
	/// this isn't going to be mutex guarded.
	/// </summary>
	long long camFeedChanges = 0;

	/// <summary>
	/// Cached width dimension of the streaming video frame.
	/// </summary>
	int streamWidth = -1;

	/// <summary>
	/// Cached height dimension of the streaming video frame.
	/// </summary>
	int streamHeight = -1;

	/// <summary>
	/// The last known connection state.
	/// 
	/// This will only be set by the CamStreamMgr but is freely 
	/// accessible to read by anything on any thread.
	/// </summary>
	State conState = State::Unknown;


public:
	~CamStreamMgr();

	/// <summary>
	/// The thread loop to poll camera frames. This is expected to run in the 
	/// background, once booted. Afterwards, it will only end after the app
	/// is closed.
	/// </summary>
	void ThreadFn();

	/// <summary>
	/// Perform image processing on the target image.
	/// 
	/// Note that the parameter and return value can be the same, but 
	/// they don't have to be. And it's expected that the shared pointer
	/// class will perform all the memory management needs.
	/// </summary>
	/// <param name="inImg">The image to process.</param>
	/// <returns>The process image.</returns>
	cv::Ptr<cv::Mat> ProcessImage(cv::Ptr<cv::Mat> inImg);

	/// <summary>
	/// Startup the working thread.
	/// </summary>
	void BootConnectionToCamera();

	/// <summary>
	/// Get access to the shared pointer of the last polled image.
	/// 
	/// This function will use imageAccess for thread saftey.
	/// </summary>
	/// <returns>
	/// The last polled image. This pointer can be null if an image
	/// has not been polled yet.
	/// </returns>
	cv::Ptr<cv::Mat> GetCurrentFrame();

	/// <summary>
	/// Set the current cached frame.
	/// 
	/// The frame cache is used to pass images between the CamStreamMgr and
	/// other threads that will request to see the current one.
	/// </summary>
	/// <param name="mat">The frame to cache.</param>
	/// <returns></returns>
	bool SetCurrentFrame(cv::Ptr<cv::Mat> mat);

	/// <summary>
	/// Shutdown the camera manager.
	/// 
	/// This should be called only once, at the end of the app's
	/// lifetime.
	/// </summary>
	/// <returns>True, if successful.</returns>
	bool Shutdown();

	inline State GetState() 
	{ return this->conState; }

	/// <summary>
	/// Toggle the polling method to be the test image.
	/// </summary>
	void ToggleTesting();

	/// <summary>
	/// Queue a request to save the next polled frame as a snapshot.
	/// </summary>
	/// <param name="filename">The filename to save.</param>
	/// <returns>
	/// The request object. This can be kept and observed to view the success
	/// status when the request is fullfilled.
	/// </returns>
	SnapRequest::SPtr RequestSnapshot(const std::string& filename);

	/// <summary>
	/// Clear all currently queued snapshot requests.
	/// </summary>
	void ClearSnapshotRequests();

private:
	/// <summary>
	/// Adjust the streaming state to disable streaming.
	/// </summary>
	/// <param name="deactivateShould">
	/// If true, the shouldBeStreaming will also be modified. This should NOT
	/// be set if the streaming needs to be disabled (because of an error) but 
	/// the application still wants to stream.</param>
	void _DeactivateStreamState(bool deactivateShould = false);

	CamStreamMgr();
};