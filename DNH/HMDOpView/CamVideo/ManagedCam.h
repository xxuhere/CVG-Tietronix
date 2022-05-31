#pragma once

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "SnapRequest.h"
#include "VideoRequest.h"
#include "../Utils/VideoPollType.h"
#include "../Utils/cvgCamFeedSource.h"
#include "../Utils/yen_threshold.h"

#include "CamImpl/ICamImpl.h"

#include <mutex>
#include <thread>
#include <memory>
#include <vector>

enum StreamParams
{
	StaticThreshold
};

/// <summary>
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
class ManagedCam
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
	

public:

	/// <summary>
	/// The camera id to differentiate it from other cameras.
	/// 
	/// Do NOT confuse this with the OpenCV VideoCapture ID.
	/// </summary>
	int cameraId = -1;

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
	/// The polling implementation to use. Depending on what this is
	/// set to, will determine what subclass is instanciated for
	/// this->currentImpl.
	/// </summary>
	VideoPollType pollType = VideoPollType::OpenCVUSB_Idx;

	/// <summary>
	/// A cache of the camera options to define the ManagedCam's stream
	/// locations and behaviours.
	/// </summary>
	cvgCamFeedSource camOptions;

	/// <summary>
	/// The polling implementation. See this->pollType, as well as 
	/// the various subclasses of ICamImpl for more details.
	/// </summary>
	ICamImpl* currentImpl = nullptr;

	std::string snapCaption;

private:

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
	/// Null the camera implementation.
	/// </summary>
	/// <param name="delCurrent">
	/// If true, delete the current implementation.
	/// </param>
	/// <param name="resetPollTy">
	/// If true, reset the poll type to VideoPollType::Deactivated.
	/// </param>
	void _ClearImplementation(bool delCurrent = true, bool resetPollTy = true);

	/// <summary>
	/// Switch the camera polling implementation method.
	/// </summary>
	/// <param name="newImplType">The new polling type.</param>
	/// <param name="delCurrent">
	/// If true, delete the old camera type.
	/// </param>
	/// <returns>True if successful, else false.</returns>
	bool SwitchImplementation(VideoPollType newImplType, bool delCurrent = true);

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

public:

	ManagedCam(VideoPollType pt, int camId, const cvgCamFeedSource& camOptions);
	~ManagedCam();

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
	/// <returns>
	/// The request object. This can be kept and observed to view the success
	/// status when the request is fullfilled.
	/// </returns>
	SnapRequest::SPtr RequestSnapshot(const std::string& filename);

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
	void ThreadFn(int camIdx);

	/// <summary>
	/// Initialize the polling thread loop.
	/// </summary>
	/// <param name="camIdx">
	/// Not functional. Used to identify the camera thread during debugging.
	/// </param>
	/// <returns></returns>
	bool BootupPollingThread(int camIdx);

	/// <summary>
	/// Query if the camera settings are set for the image feed to go through
	/// thresholding image processing.
	/// </summary>
	bool IsThresholded();

	/// <summary>
	/// Adjust the streaming state to disable streaming.
	/// </summary>
	/// <param name="deactivateShould">
	/// If true, the shouldBeStreaming will also be modified. This should NOT
	/// be set if the streaming needs to be disabled (because of an error) but 
	/// the application still wants to stream.</param>
	void _DeactivateStreamState(bool deactivateShould = false);

	/// <summary>
	/// Set the polling type of the camera.
	/// </summary>
	/// <param name="pollTy">The polling type.</param>
	void SetPoll(VideoPollType pollTy);

	/// <summary>
	/// Get the image processing algorithm.
	/// </summary>
	ProcessingType GetProcessingType() const;

	/// <summary>
	/// Set the image processing algorithm.
	/// </summary>
	/// <return>
	/// True, if successful. Currently always returns true, but added in anticipation
	/// of image processing modes that may have regulations on when switching is 
	/// successful/valid.
	/// </return>
	bool SetProcessingType(ProcessingType pt);

	float GetFloat( StreamParams paramid);

	bool SetFloat( StreamParams paramid, float value);

	void SetSnapCaption(const std::string& caption);
	

	//////////////////////////////////////////////////
	//
	//		IMAGE PROCESSING METHODS
	//
	//////////////////////////////////////////////////

	cv::Ptr<cv::Mat> ImgProc_Simple(cv::Ptr<cv::Mat> src, int threshold);

	///<summary
	/// Preform thresholding on the target image
	/// Should return a black and white image.
	/// 
	/// Note that the parameter and return value can
	/// but dont have to be the same.
	///  
	/// It's expected that teh shared pointer class 
	/// will preform all memory management needs
	/// </summary>
	/// <param name="src"> The image to threshold</param>
	/// <returns></returns>
	cv::Ptr<cv::Mat> ImgProc_YenThreshold(cv::Ptr<cv::Mat> src, bool compressed);

	cv::Ptr<cv::Mat> ImgProc_TwoStDevFromMean(cv::Ptr<cv::Mat> src);

	
};