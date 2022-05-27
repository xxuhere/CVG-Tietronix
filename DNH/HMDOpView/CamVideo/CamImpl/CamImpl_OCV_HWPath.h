#pragma once

#include "ICamImpl.h"
#include "CamImpl_OpenCVBase.h"

// This CamImpl_OpenCVBase subclass will no doubt resemble the Web subclass.
//
// This both is, and isn't a coincidence.
// But, its intent and purpose is different from Web/RTSP streaming, so it 
// should NOT be unified with the CamImpl_OCV_Web subclass.
//
// This class is only relevant for Linux, but can compile for windows - which
// we allow as less code branching means less maintenance (in theory).
class CamImpl_OCV_HWPath : public CamImpl_OpenCVBase
{
private:
	/// <summary>
	/// The device path to the hardware.
	/// 
	/// This value - and hence this OpenCVBase subclass, only makes sense
	/// for Linux.
	/// </summary>
	std::string path;

protected:
	cv::VideoCapture * CreateVideoCapture() override;
	bool PullOptions(const cvgCamFeedLocs& opts) override;

public:
	CamImpl_OCV_HWPath(const std::string& path);
	VideoPollType PollType() override;

	/// <summary>
	/// Change the path used for polling.
	/// </summary>
	/// <param name="newPath">The new path to use.</param>
	/// <param name="cannotBeRunning">
	/// If true, the function will be set to return an error if the camera
	/// is currently active. This is more to telegraph intent, as there is
	/// no guarantee that a camera switch will be successful if the polling is
	/// active and cannotBeRunning is false.
	/// </param>
	/// <returns>True, if the value was successfully changed.</returns>
	bool ChangeDeviceID(const std::string& newPath, bool cannotBeRunning = true);
};