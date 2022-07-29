#include "CamImpl_OCV_USB.h"


VideoPollType CamImpl_OCV_USB::PollType()
{
	return VideoPollType::OpenCVUSB_Idx;
}

cv::VideoCapture* CamImpl_OCV_USB::CreateVideoCapture()
{
	this->AssertStreamNull();
	return new cv::VideoCapture(this->deviceID);
}

CamImpl_OCV_USB::CamImpl_OCV_USB(int deviceID)
{
	this->deviceID = deviceID;
}

bool CamImpl_OCV_USB::ChangeDeviceID(
	int newDeviceID, 
	bool cannotBeRunning)
{
	if(cannotBeRunning == true)
	{
		if(this->IsStreamAllocated())
			return false;
	}

	this->deviceID = newDeviceID;
	return true;
}

bool CamImpl_OCV_USB::PullOptions(const cvgCamFeedLocs& opts)
{
	this->ICamImpl::PullOptions(opts);

	this->deviceID = opts.camIndex;
	return true;
}