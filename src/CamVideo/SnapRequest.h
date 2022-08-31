#pragma once
#include <string>
#include <memory>
#include <unordered_map>
class CamStreamMgr;

/// <summary>
/// The representation of a camera picture request for CamStreamMgr.
/// </summary>
class SnapRequest
{
	friend class IManagedCam;

public:

	/// <summary>
	/// The specification of what kind of snapshot to target saving.
	/// </summary>
	enum class ProcessType
	{
		/// <summary>
		/// The snapshot must have image processing.
		/// Only save the snapshots if it's an image processed version.
		/// </summary>
		HasTo,

		/// <summary>
		/// The snapshot cannot have images processing.
		/// Only save the snapshots if it's the raw image WITHOUT image processing.
		/// </summary>
		Cannot,

		/// <summary>
		/// Try to save an image processed version, but if the feed isn't
		/// image processed, just save a raw version.
		/// </summary>
		Indifferent
	};

	/// <summary>
	/// The status of the request.
	/// </summary>
	enum class Status
	{
		Unknown,

		/// <summary>
		/// The SnapRequest is a queued request in the 
		/// CamStreamMgr system.
		/// </summary>
		Requested,

		/// <summary>
		/// The request has been successfully filled. A valid image
		/// should be expected at the requested filepath.
		/// </summary>
		Filled,

		/// <summary>
		/// The request has resulted in an error. See 
		/// err for more information.
		/// </summary>
		Error
	};


private:
	// There's no making this object directly. Only through MakeRequest(),
	// and really that's probably from CamStreamMgr.
	SnapRequest();

private:
	/// <summary>
	/// The location to save the file.
	/// </summary>
	std::string filename;

	/// <summary>
	/// Matches CamStreamMgr.camFeedChanges value of the camera
	/// it is.
	/// </summary>
	long long frameID = -1;

	/// <summary>
	/// The cameara manager id of the camera handling the request.
	/// </summary>
	int camId = -1;

	/// <summary>
	/// The status of the request.
	/// </summary>
	Status status = Status::Unknown;

	ProcessType processType;

public:
	inline std::string Filename() const
	{ return this->filename; }

public:
	inline Status GetStatus() 
	{return this->status; }

	/// <summary>
	/// The error. Only set if the status is Status::Error.
	/// </summary>
	std::string err;

	typedef std::shared_ptr<SnapRequest> SPtr;

public:
	bool Cancel();

	/// <summary>
	/// Utility function to create a shared pointer to a SnapRequest.
	/// </summary>
	/// <param name="filename">The request's filename.</param>
	/// <param name="processType>The requirement for what stage of processing to record</param>
	/// <returns>The created request.</returns>
	static SPtr MakeRequest(const std::string& filename, ProcessType pt);

	/// <summary>
	/// Generate a SnapRequest containing an error state.
	/// </summary>
	/// <param name="err">The error.</param>
	/// <param name="filename">
	/// The requested filename that generated the error.
	/// This can be set to an empty string if not relevant.
	/// </param>
	/// <returns>The created error.</returns>
	static SPtr MakeError(const std::string& err, const std::string& filename);
};