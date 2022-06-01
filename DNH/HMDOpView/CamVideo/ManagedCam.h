#pragma once
#include "IManagedCam.h"

/// <summary>
/// A video streaming (usually from a camera/webcam) class. This system
/// will poll a video stream (See ICamImpl for more details) in a thread
/// as well as provide other services involved with polling the video
/// stream:
/// - Handling snapshot requests (See SnapRequest)
/// - Handling video saving requests (See VideoRequest)
/// - Staging polled image frames to other systems, such as rendering
/// systems.
/// - Performing the pipeline for image processing.
/// 
/// Note this class should not be used directly by outside calling 
/// code. Instead, access should be limited by using the public member
/// functions provided by the public functions of CamStreamMgr.
/// </summary>
class ManagedCam : public IManagedCam
{
	friend class CamStreamMgr;

public:

	/// <summary>
	/// The camera id to differentiate it from other cameras.
	/// 
	/// Do NOT confuse this with the OpenCV VideoCapture ID.
	/// </summary>
	int cameraId = -1;

	

	/// <summary>
	/// The polling implementation to use. Depending on what this is
	/// set to, will determine what subclass is instanciated for
	/// this->currentImpl.
	/// </summary>
	VideoPollType pollType = VideoPollType::OpenCVUSB_Idx;

	/// <summary>
	/// A cache of the camera options to define the ManagedCam's stream
	/// locations and behaviours.
	/// </summary>
	cvgCamFeedSource camOptions;

	/// <summary>
	/// The polling implementation. See this->pollType, as well as 
	/// the various subclasses of ICamImpl for more details.
	/// </summary>
	ICamImpl* currentImpl = nullptr;

protected:

	/// <summary>
	/// Null the camera implementation.
	/// </summary>
	/// <param name="delCurrent">
	/// If true, delete the current implementation.
	/// </param>
	/// <param name="resetPollTy">
	/// If true, reset the poll type to VideoPollType::Deactivated.
	/// </param>
	void _ClearImplementation(bool delCurrent = true, bool resetPollTy = true);

	/// <summary>
	/// Switch the camera polling implementation method.
	/// </summary>
	/// <param name="newImplType">The new polling type.</param>
	/// <param name="delCurrent">
	/// If true, delete the old camera type.
	/// </param>
	/// <returns>True if successful, else false.</returns>
	bool SwitchImplementation(VideoPollType newImplType, bool delCurrent = true);

	cv::Ptr<cv::Mat> ProcessImage(cv::Ptr<cv::Mat> inImg) override;

	void _DeactivateStreamState(bool deactivateShould = false) override;

public:

	ManagedCam(VideoPollType pt, int camId, const cvgCamFeedSource& camOptions);
	~ManagedCam();

	void ThreadFn(int camIdx) override;

	/// <summary>
	/// Query if the camera settings are set for the image feed to go through
	/// thresholding image processing.
	/// </summary>
	bool IsThresholded();

	bool UsesImageProcessingChain() override;

	/// <summary>
	/// Set the polling type of the camera.
	/// </summary>
	/// <param name="pollTy">The polling type.</param>
	void SetPoll(VideoPollType pollTy);

	/// <summary>
	/// Get the image processing algorithm.
	/// </summary>
	ProcessingType GetProcessingType() const;


	/// <summary>
	/// Set the image processing algorithm.
	/// </summary>
	/// <return>
	/// True, if successful. Currently always returns true, but added in anticipation
	/// of image processing modes that may have regulations on when switching is 
	/// successful/valid.
	/// </return>
	bool SetProcessingType(ProcessingType pt);

	float GetFloat( StreamParams paramid) override;

	bool SetFloat( StreamParams paramid, float value) override;
	
	CamType GetCamType() override;

	int GetID() const override;

	std::string GetStreamName() const override;

	//////////////////////////////////////////////////
	//
	//		IMAGE PROCESSING METHODS
	//
	//////////////////////////////////////////////////

	cv::Ptr<cv::Mat> ImgProc_Simple(cv::Ptr<cv::Mat> src, double threshold);

	///<summary
	/// Preform thresholding on the target image
	/// Should return a black and white image.
	/// 
	/// Note that the parameter and return value can
	/// but dont have to be the same.
	///  
	/// It's expected that teh shared pointer class 
	/// will preform all memory management needs
	/// </summary>
	/// <param name="src"> The image to threshold</param>
	/// <returns></returns>
	cv::Ptr<cv::Mat> ImgProc_YenThreshold(cv::Ptr<cv::Mat> src, bool compressed);

	cv::Ptr<cv::Mat> ImgProc_TwoStDevFromMean(cv::Ptr<cv::Mat> src);

};