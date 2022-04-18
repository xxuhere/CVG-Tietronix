#pragma once
#include <string>
#include <memory>
#include <unordered_map>
class CamStreamMgr;

/// <summary>
/// The representation of a camera picture request
/// for CamStreamMgr.
/// </summary>
class SnapRequest
{
	friend class ManagedCam;

public:

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
	static SPtr MakeRequest(const std::string& filename);
};