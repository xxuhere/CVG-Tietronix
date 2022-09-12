#pragma once

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "SnapRequest.h"
#include "VideoRequest.h"
#include "../Utils/VideoPollType.h"
#include "../Utils/cvgCamFeedSource.h"
#include "../Utils/cvgGrabTimer.h"

#include "CamImpl/ICamImpl.h"

#include <mutex>
#include <thread>
#include <memory>
#include <vector>

#include "../DicomUtils/DicomInjector.h"

/// <summary>
/// Parameter ID values for ALL items that can be used for 
/// GetFloat() or SetFloat(), for ALL IManagedCam subclasses.
/// </summary>
enum StreamParams
{
	// Alpha blending amount
	Alpha,

	/// <summary>
	/// For simple thresholding image processing. The pixel value to threshold.
	/// </summary>
	StaticThreshold,

	/// <summary>
	/// For composite video feeds, the width of the video to save.
	/// </summary>
	CompositeVideoWidth,

	/// <summary>
	/// For composite video feeds, the height of the video to save.
	/// </summary>
	CompositeVideoHeight
};

/// <summary>
/// Enumeration of types to allow 
/// </summary>
enum CamType
{
	/// <summary>
	/// Should never be used, represents an error.
	/// </summary>
	InvalidCam,

	/// <summary>
	/// Represents a camera stream.
	/// </summary>
	VideoFeed,

	/// <summary>
	/// Represents a special camera type, such as the composite video
	/// feed.
	/// </summary>
	SpecialType
};

/// <summary>
/// Special camera IDs. Except for ErrorCode, these are special
/// video feeds that can be used as id values for the 
/// CamStreamMgr interface functions.
/// 
/// These will be negative as positive values are reserved for
/// real cameras (ManagedCam objects).
/// 
/// For more details, see the member functions of CamStreamMgr
/// that have an 'int id' or 'int idx' parameter.
enum SpecialCams
{
	/// <summary>
	/// Not a usable video feed. Represents an invalid video feed
	/// from an error.
	/// </summary>
	ErrorCode = -1,

	/// <summary>
	/// The composited video feed. See ManagedComposite.h for its
	/// implementation.
	/// </summary>
	Composite = -2
};

/// <summary>
/// The base class for various individual video feed. 
/// A video streaming (usually from a camera/webcam) class. This system
/// will poll a video stream (See ICamImpl for more details) in a thread
/// as well as provide other services involved with polling the video
/// stream:
/// - Handling snapshot requests (See SnapRequest)
/// - Handling video saving requests (See VideoRequest)
/// - Staging polled image frames to other systems, such as rendering
/// systems.
/// - Performing the pipeline for image processing.
/// 
/// Note this class should not be used directly by outside calling 
/// code. Instead, access should be limited by using the public member
/// functions provided by the public functions of CamStreamMgr.
/// </summary>
class IManagedCam : public DicomInjector
{
	friend class CamStreamMgr;

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
		/// The manager's thread is running, but idling.
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

public:
	/// <summary>
	/// This should match with the number of times curCamFrame has been updated,
	/// so we if need track of the last time we polled curCamFrame and the 
	/// change counter, we know if we can expect a change if we poll again.
	/// 
	/// For simplicity, and because there's little threading danger doing so,
	/// this isn't going to be mutex guarded.
	/// </summary>
	long long camFeedChanges = 0;

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
	/// Mutex for thread locking BOTH activeVideoReq and videoWrite.
	/// </summary>
	std::mutex videoAccess;

	/// <summary>
	/// The request of where/how to save the incomming stream as a video file. This
	/// should be thread locked with videoAccess before using.
	/// 
	/// If non-null, the request should either be Requested or StreamingOut. All other
	/// states should cause us to loose our handle to the pointer because it's no longer
	/// of any interest to us.
	/// </summary>
	VideoRequest::SPtr activeVideoReq;

	/// <summary>
	/// The writer for saving the incomming stream as a video file. This should be 
	/// thread locked with videoAccess before using.
	/// </summary>
	cv::VideoWriter videoWrite;

	/// <summary>
	/// Timer for how much time needs to be accounted for in the recorded video. This makes
	/// sure the video being recorded matches up to real-world time, even if padded frames
	/// and frame drops occur.
	/// </summary>
	cvgGrabTimer videoGrabTimer;

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
	/// Is the manger streaming?
	/// </summary>
	bool _isStreamActive = false;

	/// <summary>
	/// The working thread for polling and other CamStreamMgr
	/// tasks. This will only be non-null if 
	/// </summary>
	std::thread* camStreamThread = nullptr;

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

	/// <summary>
	/// The number of milliseconds between the last two frames.
	/// </summary>
	int msInterval = 0;

	/// <summary>
	/// The number of frames processed in the stream. Only used
	/// for diagnostic purposes.
	/// </summary>
	int streamFrameCt = 0;

	/// <summary>
	/// The amount to blend in compositing - may only apply to 
	/// thresholded feeds.
	/// </summary>
	float alpha = 1.0f;

protected:
	/// <summary>
	/// Flag the thread for shutdown. Note that this will not actually
	/// fully shutdown the thread, just initiate the shutdown process.
	/// 
	/// When the function exits, the thread will probably still be running
	/// and the program will need to wait for it to shut down and then call
	/// _JoinThread to complete the entire process of ending the thread.
	/// </summary>
	/// <returns>True if successful. Else, false.</returns>
	bool ShutdownThread();

	/// <summary>
	/// After a thread is shutdown, _JoinThread should be called to join
	/// the thread and clean up thread resources.
	/// </summary>
	/// <returns>True if successful. Else, false.</returns>
	bool _JoinThread();

	/// <summary>
	/// Close the video feed without locking the video mutex. The only reason
	/// to do this is if the mutex is already locked.
	/// </summary>
	/// <returns>True if the video feed was succesfully closed. Else, false.</returns>
	bool _CloseVideo_NoMutex();

	/// <summary>
	/// Standard pipeline of handling streamed images, regardless
	/// of how we obtained (polled) them.
	/// </summary>
	/// <param name="ptr">
	/// The shared pointer containing the image to handle.
	/// </param>
	/// <returns>True if image was handled.</returns>
	bool _FinalizeHandlingPolledImage(cv::Ptr<cv::Mat> ptr);

	/// <summary>
	/// Add another frame to the video being saved.
	/// 
	/// THREAD WARNING: When the function is called
	/// - it assumes the image is threadsafe.
	/// - it assumes the videoAccess mutex is locked.
	/// - it assumes the activeVideoRequest is properly set.
	/// 
	/// Function will automatically shut down the video stream and
	/// recording process if it detects any request errors or state errors.
	/// </summary>
	/// <param name="img">The OpenCV image to add to the video.</param>
	/// <returns>
	/// True if the frame was added to the video file successfully, else
	/// false.
	/// </returns>
	bool _DumpImageToVideofile(const cv::Mat& img);

	/// <summary>
	/// Adjust the streaming state to disable streaming.
	/// </summary>
	/// <param name="deactivateShould">
	/// If true, the shouldBeStreaming will also be modified. This should NOT
	/// be set if the streaming needs to be disabled (because of an error) but 
	/// the application still wants to stream.</param>
	virtual void _DeactivateStreamState(bool deactivateShould = false);

	/// <summary>
	/// Perform image processing on the target image.
	/// 
	/// Note that the parameter and return value can be the same, but 
	/// they don't have to be. And it's expected that the shared pointer
	/// class will perform all the memory management needs.
	/// </summary>
	/// <param name="inImg">The image to process.</param>
	/// <returns>The process image.</returns>
	virtual cv::Ptr<cv::Mat> ProcessImage(cv::Ptr<cv::Mat> inImg) = 0;

	virtual void _EndShutdown();

public:

	/// <summary>
	/// Set a parameter.
	/// </summary>
	/// <param name="paramid">
	/// The parameter to set.
	/// 
	/// The values that are accepted will depend on the subclass implementation.
	/// </param>
	/// <returns>
	/// The retrieved value.
	/// If the value is not supported. -1 is returned.
	/// 
	/// In the future, this may change to a floating point error code; eg, NaN.
	/// </returns>
	virtual float GetFloat( StreamParams paramid);

	/// <summary>
	/// Set a parameter.
	/// </summary>
	/// <param name="paramid">
	/// The parameter to set.
	/// 
	/// The values that are relevant and accepted will depend on the subclass
	/// implementation.
	/// </param>
	/// <param name="value">The value to set the parameter to.</param>
	/// <returns>
	/// True if the value was successfully set. Else, there was an issue with
	/// the value, OR the parameter is unsupported.
	/// </returns>
	virtual bool SetFloat( StreamParams paramid, float value);

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

	inline State GetState() 
	{ return this->conState; }

	/// <summary>
	/// Queue a request to save the next polled frame as a snapshot.
	/// </summary>
	/// <param name="filename">The filename to save.</param>
	/// <param name="procType">Selection to save the image processed version or raw version.</param>
	/// <returns>
	/// The request object. This can be kept and observed to view the success
	/// status when the request is fullfilled.
	/// </returns>
	SnapRequest::SPtr RequestSnapshot(
		const std::string& filename, 
		SnapRequest::ProcessType procType);

	/// <summary>
	/// Clear all currently queued snapshot requests.
	/// </summary>
	void ClearSnapshotRequests();

	/// <summary>
	/// Request saving the stream to a video.
	/// </summary>
	/// <param name="filename">The filename to save the video to.</param>
	/// <returns>The VideoRequest representing the request.</returns>
	VideoRequest::SPtr OpenVideo(const std::string& filename);

	/// <summary>
	/// Close the video that's currently being recorded.
	/// Obviously only useful if a video is currently being recorded. Note
	/// that this will also update the state of the VideoRequest to be 
	/// Status::Closed.
	/// </summary>
	/// <returns>
	/// True if a recording session was successfully closed.
	/// </returns>
	bool CloseVideo();

	/// <summary>
	/// Query if the ManagedCam is currently saving polled frames to a video.
	/// </summary>
	bool IsRecordingVideo();

	/// <summary>
	/// If the ManagedCam is currently saving polled frames to a video, query
	/// the video filename.
	/// </summary>
	std::string VideoFilepath();

	/// <summary>
	/// The thread loop to poll camera frames. This is expected to run in the 
	/// background, once booted. Afterwards, it will only end after the app
	/// is closed.
	/// </summary>
	/// <param name="camIdx">
	/// Not functional. Used to identify the camera thread during debugging.
	/// </param>
	virtual void ThreadFn(int camIdx) = 0;

	/// <summary>
	/// Initialize the polling thread loop.
	/// </summary>
	/// <param name="camIdx">
	/// Not functional. Used to identify the camera thread during debugging.
	/// </param>
	/// <returns></returns>
	bool BootupPollingThread(int camIdx);

	/// <summary>
	/// Allows subclasses to tell the general IManagedCam functions what type
	/// of video stream is being implemented.
	/// </summary>
	virtual CamType GetCamType() = 0;

	/// <summary>
	/// Gets the ID of the IManagedCam. This will either be the index of the
	/// camera, for normal feeds, or a value from the SpecialCams enum.
	/// </summary>
	virtual int GetID() const = 0;

	/// <summary>
	/// Gets the string name of the IManagedCam, mainly used to decorate 
	/// filenames and convert SpecialCams ids into readable values.
	/// </summary>
	virtual std::string GetStreamName() const = 0;

	/// <summary>
	/// Allow subclasses to tell the general IManagedCam functions if the 
	/// video feed is currently applying an image processing chain.
	/// </summary>
	virtual bool UsesImageProcessingChain() = 0;

public:
	/// <summary>
	/// Unified way for how watermark annotations should be applied
	/// to images.
	/// </summary>
	/// <param name="mat">The image to apply the watermark to.</param>
	/// <param name="text">The watermark text.</param>
	void ApplySnapshotWatermarkText(cv::Mat& mat, const std::string& text);

public:
	//////////////////////////////////////////////////
	//
	//	DicomInjector FUNCTIONS
	//
	//////////////////////////////////////////////////

	// Set to do-nothing, but open for subclasses implement on-top.
	//
	// This will NOT be added to the singleton DicomInjectorSet. It
	// will inject in the polling loop, only for assets that its 
	// directly saving.
	virtual void InjectIntoDicom(DcmDataset* dicomData);

public:
	//////////////////////////////////////////////////
	//
	//	Misc Utilities
	//
	//////////////////////////////////////////////////

	/// <summary>
	/// Utility function to perform Dicom saving of a request, as well
	/// as managing updating the status properly and rejecting cancelled
	/// requests.
	/// </summary>
	/// <param name="imgMat">The image to save.</param>
	/// <param name="cam">The camera that the image came from.</param>
	/// <param name="snreq">The snapshot request being processed.</param>
	/// <param name="camFeedChanges">
	/// The counter for the number of pictures taken during the
	/// session application.</param>
	/// <returns>True if successful, else false.</returns>
	static bool SaveMatAsDicomJpeg_HandleReq(
		cv::Ptr<cv::Mat> imgMat, 
		IManagedCam* cam, 
		SnapRequest::SPtr snreq,
		int camFeedChanges);
};


/// <summary>
/// Utility function to insert an arbitrary string attribute into the 
/// DCM_AcquisitionContextSequence, which is used for storing arbitrary 
/// camera properties.
/// </summary>
/// <param name="dicomData">The dicom data to save to.</param>
/// <param name="key">The attribute key.</param>
/// <param name="value">The attribute value.</param>
/// <returns>True if added, else an error occured.</returns>
bool InsertAquisitionContextInfo(
	DcmDataset* dicomData,
	const std::string& key, 
	const std::string& value);