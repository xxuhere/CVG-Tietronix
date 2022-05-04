#pragma once

#include <vector>
#include <string>
#include <mutex>

#include "ICamImpl.h"

struct RPiYUVState;
struct MMAL_PORT_T;
struct MMAL_BUFFER_HEADER_T;

class CamImpl_MMAL : public ICamImpl
{	
private:
	/// <summary>
	/// A count of the total number of CamImpl_MMAL objects initialized.
	/// </summary>
	static int instsInit;
	
private:
	/// <summary>
	/// Cache of the current device path being used.
	/// </summary>
	std::string devPath;

	/// <summary>
	/// YUV stuff, hidden behind a Pimpl.
	/// </summary>
	RPiYUVState * state = nullptr;

	/// <summary>
	/// Used to control when to stop polling camera data in the
	/// MMAL stream callback.
	/// </summary>
	bool polling = false;
	
	/// <summary>
	/// The last polled image data from the callback.
	/// </summary>
	cv::Ptr<cv::Mat> lastPolled;
	
	/// <summary>
	/// The thread guard for lastPolled. Note that this isn't for
	/// the standard thread protection between the camera polling and
	/// the main UI, but between the MMAL callback and the CamImpl.
	/// </summary>
	std::mutex mutPolled;

	/// <summary>
	/// The video port for the camera being polled.
	/// </summary>
	MMAL_PORT_T* camVideoPort = nullptr;
	
private:
	/// <summary>
	/// Certain things between VideoCore, Broadcom and RaspberryPi require
	/// certain things to be done exactly once per program, before the
	/// program is used.
	/// </summary>
	static void InitPrereqs();

public:
	/// <summary>
	/// Camera buffer callback for when the video has new 
	/// streaming data to be processed. 
	/// </summary>
	static void _CameraBufferCallback(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T* buffer);

	/// <summary>
	/// Camera control callback for when there is a change 
	/// in the camera's control parameters.
	/// </summary>
	static void _CameraControlCallback(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer);

protected:

	bool InitializeImpl() override;
	bool ShutdownImpl() override;
	bool ActivateImpl() override;
	bool DeactivateImpl() override;
	cv::Ptr<cv::Mat> PollFrameImpl() override;

	bool _ShutdownGlobal();
	bool _ShutdownGlobal(MMAL_STATUS_T status);
public:

	CamImpl_MMAL(const std::string& devPath);
	
	VideoPollType PollType() override;
	bool IsValid() override;
	bool PullOptions(const cvgCamFeedLocs& opts) override;
};