#pragma once

#include "ManagedCam.h"
#include "../Utils/VideoPollType.h"
#include "../Utils/cvgCamFeedSource.h"

class ManagedComposite;

// NOTE: The terms camera stream and video streams are used 
// somewhat synonamously for the comments and elements in this
// system. The streams are really video streams - but often times
// they will be specificly video streams from a camera.
//
// If this becomes a problem, a refactor to canonicalize these
// terms may be necessary.

/// <summary>
/// A system that manages multiple cameras and streaming video systems.
///
/// Outside code should not touch those camera (ManagedCam) and video
/// systems. The whole point of this system is that it's an intermediary
/// delegating authority.
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
	/// The composite video feed, accessible by using the special
	/// id SpecialCams::Composite
	/// </summary>
	ManagedComposite * composite;

	/// <summary>
	/// Ensure nothing is attempting to modify the cameras
	/// and read from them at the same time.
	/// </summary>
	std::mutex camAccess;

	int activeIndex = -1;

private:
	IManagedCam* _GetIManaged(int idx);

	ManagedCam* _GetManaged(int idx);

public:
	~CamStreamMgr();

	/// <summary>
	/// Startup the working thread.
	/// </summary>
	bool BootConnectionToCamera(
		int camCt, 
		VideoPollType pt = VideoPollType::Deactivated);

	/// <summary>
	/// Initialize all the cameras.
	/// </summary>
	/// <param name="sources">The properties of the cameras to initialize.</param>
	/// <returns>True, if successful.</returns>
	bool BootConnectionToCamera(const std::vector<cvgCamFeedSource>& sources);

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
	void SetPollType(int idx, VideoPollType pty);

	/// <summary>
	/// Clear all snapshot request for a specific camera.
	/// </summary>
	/// <param name="idx">The camera index.</param>
	void ClearSnapshotRequests(int idx);

	/// <summary>
	/// Clear all snapshot requests of all videos.
	/// </summary>
	void ClearAllSnapshotRequests();

	/// <summary>
	/// Queue a snapshot request for the new camera frame.
	/// </summary>
	/// <param name="idx">The video index to queue.</param>
	/// <param name="filename">
	/// The filename to save the snapshot as.
	/// Do not include a file extension, (for now) it is assumed to be a Dicom file.
	/// </param>
	/// <param name="procType">The processing type to save.</param>
	/// <returns>A handle to the generated snapshot request.</returns>
	SnapRequest::SPtr RequestSnapshot(
		int idx, 
		const std::string& filename, 
		SnapRequest::ProcessType procType);

	/// <summary>
	/// Queue a snapshot request for all camera streams.
	/// </summary>
	/// <param name="filenameBase">
	/// The base filename to save the snapshot as. More will be added to the
	/// filename to identify the camera stream. 
	/// Do not include a file extention, (for now) it is assumed to be a Dicom file.
	/// </param>
	/// <returns>
	/// Handles to the generated snapshot requests. There should be one for every
	/// known camera stream.
	/// </returns>
	std::vector<SnapRequest::SPtr> RequestSnapshotAll(const std::string& filenameBase);

	/// <summary>
	/// Requests a video stream to be recorded to a video file.
	/// </summary>
	/// <param name="idx">The id of the video to request the recording for.</param>
	/// <param name="filename">The video filename to record the video to.</param>
	/// <returns> The VideoRequest object related to the request.</returns>
	VideoRequest::SPtr RecordVideo(int idx, const std::string& filename);

	/// <summary>
	/// Stop recording a video stream.
	/// </summary>
	/// <param name="idx">The id of video stream to stop recording.</param>
	/// <returns>True, if the request to stop the recording was successful.</returns>
	bool StopRecording(int idx);

	/// <summary>
	/// Query if a video stream is currenly saving to a file.
	/// </summary>
	/// <param name="idx">The id of the video stream to query.</param>
	/// <returns>
	/// True, if the specified camera is recording. Else, false.
	/// </returns>
	bool IsRecording(int idx);

	/// <summary>
	/// Query if a video stream is set to be image processed.
	/// </summary>
	/// <param name="idx">The id of the video stream to query.</param>
	/// <returns>
	/// True, if the video stream is set to be thresholded. Else, false.
	/// </returns>
	bool IsThresholded(int idx);

	/// <summary>
	/// Check if a camera is recording a video feed. and if so, check 
	/// the filename being recorded to.
	/// </summary>
	/// <param name="idx">The </param>
	/// <returns>
	/// The video filename the specified camera is being recorded to,
	/// or an empty string.
	/// </returns>
	std::string RecordingFilename(int idx);

	/// <summary>
	/// Query the last frame's processing time, in milliseconds.
	/// </summary>
	/// <param name="idx">The camera index to query.</param>
	int GetMSFrameTime(int idx);

	/// <summary>
	/// Query the number of frames processed in the current camera stream.
	/// </summary>
	/// <param name="idx">The camera index to query.</param>
	int GetStreamFrameCt(int idx);

	/// <summary>
	/// Query the processing type of a camera stream.
	/// </summary>
	/// <param name="idx">The camera index to query.</param>
	ProcessingType GetProcessingType(int idx);

	/// <summary>
	/// Set the processing type of a camera stream.
	/// </summary>
	/// <param name="idx">The camera stream to set.</param>
	/// <param name="pt">The camera's new processing type.</param>
	bool SetProcessingType(int idx, ProcessingType pt);

	/// <summary>
	/// Get a property value.
	/// See IManagedCam::GetParam() for more details.
	/// </summary>
	/// <param name="id">The id of the camera.</param>
	/// <param name="paramid">The parameter to retrieve the value of.</param>
	/// <returns>The retrieved value.</returns>
	double GetParam(int id, StreamParams paramid);

	/// <summary>
	/// Set a float property value.
	/// See IManagedCam::SetParam() for more details.
	/// </summary>
	/// <param name="id">The id of the camera.</param>
	/// <param name="paramid">The parameter to set the value of.</param>
	/// <param name="value">The value to set.</param>
	/// <returns>
	/// true if successful. false if the paramid was unknown, or 
	/// if an error occured.
	/// </returns>
	bool SetParam(int id, StreamParams paramid, double value);

private:
	// Only the singleton systems should be in charge of its construction.
	CamStreamMgr();

public:
	static void ValidateSourcePlatforms(const std::vector<cvgCamFeedSource>& sources);
};