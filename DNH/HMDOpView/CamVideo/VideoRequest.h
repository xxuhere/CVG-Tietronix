#pragma once
#include <string>
#include <memory>

struct VideoRequest
{
	friend class ManagedCam;

public:
	/// <summary>
	/// The status of the request.
	/// </summary>
	enum class Status
	{
		Unknown,
		Requested,
		StreamingOut,
		Closed,
		Error
	};

private:
	int width;
	int height;
	int camId;
	std::string filename;
	Status status = Status::Unknown;
	bool _reqStopped = false;

private:
	// Only MakeRequest should instance these items,
	// therefor ensuring all items are contained
	// as shared pointers.
	VideoRequest(int width, int height, int camId, const std::string& filename);

public:
	std::string err;

public:
	// Read access, but no writing access, only the manager can handle that.
	inline int Width() { return this->width;}
	inline int Height() {return this->height;}
	inline int CamId() {return this->camId; }
	inline std::string Filename(){return this->filename; }
	inline Status GetStatus(){return this->status;}

	// Once the request is set, it can't be unset.
	inline void RequestStop(){this->_reqStopped = true;}

public:
	typedef std::shared_ptr<VideoRequest> SPtr;
	static SPtr MakeError( const std::string& err);
private:
	static SPtr MakeRequest(int width, int height, int camId, const std::string& filename);

};