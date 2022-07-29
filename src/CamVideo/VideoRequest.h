#pragma once

#include <string>
#include <memory>

/// <summary>
/// When a video recording request is made for a camera, a 
/// VideoRequest::SPtr object is passed back - which is 
/// implemented through the VideoRequest struct.
/// 
/// See CamStreamMgr::RecordVideo() and ManagedCam::OpenVideo() 
/// for more information.
/// 
/// The object represents a few things:
/// - The success information returned from the request function.
/// - Functionality to poll the active state of the camera's video 
/// recording related to the request.
/// - An interface for anything holding the request to stop the 
/// recording related to the request.
/// </summary>
struct VideoRequest
{
	friend class IManagedCam;

public:
	/// <summary>
	/// The status of the request.
	/// </summary>
	enum class Status
	{
		/// <summary>
		/// Unitialized state, or unhandled error state.
		/// </summary>
		Unknown,

		/// <summary>
		/// A request has been made, but managing systems (on other 
		/// threads that the request is queued in) have not processed 
		/// the request yet.
		/// </summary>
		Requested,

		/// <summary>
		/// The request has been processed, and the video file is 
		/// currently being saved.
		/// </summary>
		StreamingOut,

		/// <summary>
		/// The request was previously processed but has since been 
		/// closed.
		/// </summary>
		Closed,

		/// <summary>
		/// There was an error when attemping to process the request.
		/// </summary>
		Error
	};

private:
	/// <summary>
	/// Cached copy of the video with.
	/// </summary>
	int width;

	/// <summary>
	/// Cached copy of the video height.
	/// </summary>
	int height;

	/// <summary>
	/// Cached copy of the camera ID. 
	/// This is the same ID used as the parameter for 
	/// CamStreamMgr::RecordVideo().
	/// </summary>
	int camId;

	/// <summary>
	/// Cached copy of the video filename requested to be saved to.
	/// This is the same value used as the parameter for 
	/// CamStreamMgr::RecordVideo().
	/// </summary>
	std::string filename;

	/// <summary>
	/// The current status of the VideoRequest.
	/// </summary>
	Status status = Status::Unknown;

	/// <summary>
	/// Semaphore used to request stopping an active request.
	/// 
	/// Saving frames to the video file and any other processing 
	/// happens in the camera thread through polling at ~30fps. 
	/// Thus, the request to stop cannot be done immediately. Instead, 
	/// this flag is needed to tell the video recording to stop when 
	/// the next polling cycle occurs.
	/// </summary>
	bool _reqStopped = false;

private:
	// Only MakeRequest should instance these items, therefor 
	// ensuring all items are contained as shared pointers.
	VideoRequest(int width, int height, int camId, const std::string& filename);

public:
	/// <summary>
	/// The video request error. This is only valid if the status is
	/// set to Status::Error.
	/// </summary>
	std::string err;

public:
	// For the comments below, the term "client" reffers to the code/system/module/thread
	// that called CamStreamMgr::RecordVideo to retrieve the VideoRequest token.

	// The properties below can only be read by the user. Only the manager
	// (specificly its thread) has permission to write to these values.

	// Note that the width may not be accurate until the first frame of the
	// video has been flushed.
	inline int Width() { return this->width;}

	// Note that the height may not be accurate until the first frame of the
	// video has been flushed.
	inline int Height() {return this->height;}

	inline int CamId() {return this->camId; }
	inline std::string Filename(){return this->filename; }
	inline Status GetStatus(){return this->status;}

	/// <summary>
	/// Can be called by clients to request the video request be stopped.
	/// Once stopped, it can never be restarted - instead, a new seperate 
	/// request must be made.
	/// </summary>
	inline void RequestStop() {this->_reqStopped = true;}

public:
	typedef std::shared_ptr<VideoRequest> SPtr;

	/// <summary>
	/// Create a VideoRequest object that is set to be an error with a 
	/// specfied error message.
	/// 
	/// Unlike MakeRequest(), note that this is a public function, if 
	/// anything wants to create a non-functional error object, so be it.
	/// </summary>
	/// <param name="err">The error message.</param>
	/// <returns>The created error request.</returns>
	static SPtr MakeError( const std::string& err);

private:
	/// <summary>
	/// Create a VideoRequest object.
	/// 
	/// Note that this is a private function, meant to only be used by
	/// the friend class ManagedCam.
	/// </summary>
	/// <param name="width">The starting width cached in the request.</param>
	/// <param name="height">The starting height cached in the request.</param>
	/// <param name="camId">The camera ID the request was specified for.</param>
	/// <param name="filename">The video filename the request was specified for.</param>
	/// <returns>The created request, with the state defaulted to Unknown.</returns>
	static SPtr MakeRequest(int width, int height, int camId, const std::string& filename);

};