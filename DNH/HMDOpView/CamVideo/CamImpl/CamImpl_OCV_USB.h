#pragma once

#include "ICamImpl.h"
#include "CamImpl_OpenCVBase.h"

/// <summary>
/// CamImpl implementation for polling a USB connected webcam
/// using OpenCVs device id system.
/// </summary>
class CamImpl_OCV_USB : public CamImpl_OpenCVBase
{
private:

	/// <summary>
	/// The device id - this is the same integer ID used for
	/// the OpenCV API.
	/// </summary>
	int deviceID = -1;

protected:
	cv::VideoCapture * CreateVideoCapture() override;

public:
	CamImpl_OCV_USB(int deviceID);

	VideoPollType PollType() override;
	bool PullOptions(const cvgCamFeedLocs& opts) override;

	/// <summary>
	/// Change the device ID used for polling.
	/// </summary>
	/// <param name="newDeviceID">The new device ID to use.</param>
	/// <param name="cannotBeRunning">
	/// If true, the function will be set to return an error if the camera
	/// is currently active. This is more to telegraph intent, as there is
	/// no guarantee that a camera switch will be successful if the polling is
	/// active and cannotBeRunning is false.
	/// </param>
	/// <returns>True, if the value was successfully changed.</returns>
	bool ChangeDeviceID(int newDeviceID, bool cannotBeRunning = true);
};