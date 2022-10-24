#pragma once

#include <vector>
#include <string>
#include <mutex>

#include "ICamImpl.h"

#include "interface/vcos/vcos.h"
#include "interface/mmal/mmal.h"
#include "interface/mmal/mmal_logging.h"
#include "interface/mmal/mmal_buffer.h"
#include "interface/mmal/util/mmal_util.h"
#include "interface/mmal/util/mmal_util_params.h"
#include "interface/mmal/util/mmal_default_components.h"
#include "interface/mmal/util/mmal_connection.h"


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
	/// Cache of the current device camera ID.
	/// </summary>
	int devCamID;

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
	/// The exposure time to set. Value of 0 means to use default.
	/// </summary>
	int videoExposureTime = 0;
	
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

	/// <summary>
	/// Thread saftey for reading/writing of the cached settings below.
	/// </summary>
	std::mutex cachedSettingsMutex;
	//
	std::optional<float>	cachedExposure;
	std::optional<float>	cachedAnalogGain;
	std::optional<float>	cachedDigitalGain;
	std::optional<float>	cachedRedGain;
	std::optional<float>	cachedBlueGain;
	std::optional<uint32_t> cachedFocusPos;

	bool spamGains = false;
	std::optional<WhiteBalanceGain> whitebalanceGain;
	std::optional<CameraGain> cameraGain;
	
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
public:

	CamImpl_MMAL(int devCamID);
	
	VideoPollType PollType() override;
	bool IsValid() override;
	bool PullOptions(const cvgCamFeedLocs& opts) override;
	void DelegatedInjectIntoDicom(DcmDataset* dicomData) override;
	bool SetParam(StreamParams paramid, double value) override;

	/// <summary>
	/// Set how long the exposure time will be. This will affect both the 
	/// framerate and the shutterspeed.
	/// 
	/// Set to a value of 0 to turn off and use automatic.
	/// </summary>
	void SetExposureMicroseconds(long microseconds);

	void EnforceGainSettings();
};