#pragma once

#include "ICamImpl.h"
#include <vector>

// NOTE: This class has implementation logic, but has not been
// tested!

/// <summary>
/// A ICamImpl that mimics a video feed by sending a
/// preloaded image.
/// </summary>
class CamImpl_StaticImg : public ICamImpl
{
private:
	/// <summary>
	/// The image to load image data from.
	/// </summary>
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