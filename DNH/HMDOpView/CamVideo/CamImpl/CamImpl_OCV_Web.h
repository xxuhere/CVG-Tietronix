#pragma once

#include "ICamImpl.h"
#include "CamImpl_OpenCVBase.h"

class CamImpl_OCV_Web : public CamImpl_OpenCVBase
{
private:
	std::string url;

protected:
	cv::VideoCapture * CreateVideoCapture() override;

public:
	CamImpl_OCV_Web(const std::string& url);
	bool ChangeDeviceID(const std::string& url, bool cannotBeRunning = true);
	VideoPollType PollType() override;
	bool PullOptions(const cvgCamFeedLocs& opts) override;
};