#pragma once

#include "ICamImpl.h"
#include "CamImpl_OpenCVBase.h"

// This CamImpl_OpenCVBase subclass will no doubt resemble the Web subclass.
//
// This both is, and isn't a coincidence.
// But, its intent and purpose is different from Web/RTSP streaming, so it 
// should NOT be unified with the CamImpl_OCV_Web subclass.
class CamImpl_OCV_HWPath : public CamImpl_OpenCVBase
{
private:
	std::string path;

protected:
	cv::VideoCapture * CreateVideoCapture() override;
	bool PullOptions(const cvgCamFeedLocs& opts) override;

public:
	CamImpl_OCV_HWPath(const std::string& path);
	bool ChangeDeviceID(const std::string& newPath, bool cannotBeRunning = true);
	VideoPollType PollType() override;
};