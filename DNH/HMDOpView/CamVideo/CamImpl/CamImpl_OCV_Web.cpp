#include "CamImpl_OCV_Web.h"

VideoPollType CamImpl_OCV_Web::PollType()
{
	return VideoPollType::Web;
}

cv::VideoCapture * CamImpl_OCV_Web::CreateVideoCapture()
{
	this->AssertStreamNull();
	return new cv::VideoCapture(this->url);
}

CamImpl_OCV_Web::CamImpl_OCV_Web(const std::string& path)
{
	this->url = path;
}

bool CamImpl_OCV_Web::ChangeDeviceID(
	const std::string& newUrl, 
	bool cannotBeRunning)
{
	if(cannotBeRunning == true)
	{
		if(this->IsStreamAllocated())
			return false;
	}

	this->url = newUrl;
	return true;
}

bool CamImpl_OCV_Web::PullOptions(const cvgCamFeedLocs& opts)
{
	this->url = opts.uriSource;
	return true;
}