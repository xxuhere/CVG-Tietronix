#pragma once

#include "ICamImpl.h"
#include <vector>

class CamImpl_StaticImg : public ICamImpl
{
private:
	std::string imgPath;

protected:

	bool InitializeImpl() override;
	bool ShutdownImpl() override;
	bool ActivateImpl() override;
	bool DeactivateImpl() override;
	cv::Ptr<cv::Mat> PollFrameImpl() override;

	bool CheckImgPathExists() const;

	bool PullOptions(const cvgCamFeedLocs& opts) override;

public:
	CamImpl_StaticImg(const std::string& imgPath);
	VideoPollType PollType() override;
	bool IsValid() override;
};