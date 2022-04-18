#include "CamImpl_OCV_HWPath.h"

VideoPollType CamImpl_OCV_HWPath::PollType()
{
	return VideoPollType::OpenCVUSB_Named;
}

cv::VideoCapture * CamImpl_OCV_HWPath::CreateVideoCapture()
{
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
	this->path = opts.devicePath;
	return true;
}