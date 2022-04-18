#pragma once

#include "ICamImpl.h"
#include "CamImpl_OpenCVBase.h"

class CamImpl_OCV_USB : public CamImpl_OpenCVBase
{
private:
	int deviceID = -1;

protected:
	cv::VideoCapture * CreateVideoCapture() override;

public:
	CamImpl_OCV_USB(int deviceID);
	bool ChangeDeviceID(int newDeviceID, bool cannotBeRunning = true);
	VideoPollType PollType() override;
	bool PullOptions(const cvgCamFeedLocs& opts) override;
};