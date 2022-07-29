#pragma once

#include "ICamImpl.h"
#include "CamImpl_OpenCVBase.h"

/// <summary>
/// CamImpl implementation for polling a web streaming video
/// feed using OpenCV.
/// </summary>
class CamImpl_OCV_Web : public CamImpl_OpenCVBase
{
private:
	/// <summary>
	/// The web path to the streaming video source.
	/// </summary>
	std::string url;

protected:
	cv::VideoCapture * CreateVideoCapture() override;

public:
	CamImpl_OCV_Web(const std::string& url);
	VideoPollType PollType() override;
	bool PullOptions(const cvgCamFeedLocs& opts) override;

	/// <summary>
	/// Change the url used for streaming.
	/// </summary>
	/// <param name="newDeviceID">The new url to stream from.</param>
	/// <param name="cannotBeRunning">
	/// If true, the function will be set to return an error if the video stream
	/// is currently active. This is more to telegraph intent, as there is
	/// no guarantee that a stream switch will be successful if the polling is
	/// active and cannotBeRunning is false.
	/// </param>
	/// <returns>True, if the value was successfully changed.</returns>
	bool ChangeDeviceID(const std::string& url, bool cannotBeRunning = true);
};