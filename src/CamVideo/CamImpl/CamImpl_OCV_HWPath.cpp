#include "CamImpl_OCV_HWPath.h"
#include <iostream>//for degugging
#include <mutex>
#include <thread>

VideoPollType CamImpl_OCV_HWPath::PollType()
{
	return VideoPollType::OpenCVUSB_Named;
}

cv::VideoCapture * CamImpl_OCV_HWPath::CreateVideoCapture()
{
	///It turns out that cv::VideoCapture may not be thread-safe for V4L2 
	/// which is what the pi uses which lead to intermittent segfaults
	/// so we are going to make mutex guards if we are not in windows.
#if !_WIN32
	static std::mutex mutexReturn;
	std::lock_guard<std::mutex> guard(mutexReturn);
#endif

	this->AssertStreamNull();
	return new cv::VideoCapture(this->path);
}

CamImpl_OCV_HWPath::CamImpl_OCV_HWPath(const std::string& path)
{
	this->path = path;
}

bool CamImpl_OCV_HWPath::ChangeDeviceID(
	const std::string& newPath,
	bool cannotBeRunning)
{
	if(cannotBeRunning == true)
	{
		if(this->IsStreamAllocated())
			return false;
	}

	this->path = newPath;
	return true;
}

bool CamImpl_OCV_HWPath::PullOptions(const cvgCamFeedLocs& opts)
{
	this->ICamImpl::PullOptions(opts);

	this->path = opts.devicePath;
	return true;
}
