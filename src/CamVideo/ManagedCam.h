#pragma once
#include "IManagedCam.h"

/// <summary>
/// Subclass of IManagedCam to represent a video feed.
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

	/// <summary>
	/// Process a greyscale image to return it as a RGB-A heatmap, where
	/// the RGB is the heatmap, and the A is the binary mask of the thresholding.
	/// </summary>
	/// <param name="inImg">The greyscale image to process.</param>
	/// <returns>The alpha-channeled heatmap.</returns>
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

	double GetParam( StreamParams paramid) override;

	bool SetParam( StreamParams paramid, double value) override;
	
	CamType GetCamType() override;

	int GetID() const override;

	std::string GetStreamName() const override;

	void InjectIntoDicom(DcmDataset* dicomData) override;

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
	/// <param name="foundMinThresh">
	/// Output parameter.
	/// The threshold used, calculated with Yen's algorithm.
	/// </param>
	/// <returns>The binary mask image from the threshold.</returns>
	cv::Ptr<cv::Mat> ImgProc_YenThreshold(cv::Ptr<cv::Mat> src, bool compressed, double& foundThresh);

	/// <summary>
	/// Performs thresholding using the value found by adding 
	/// two standard deviations above the mean.
	/// </summary>
	/// <param name="src">The image to threshold.</param>
	/// <param name="foundMinThresh">
	/// Output parameter.
	/// The calculated threshold value.
	/// </param>
	/// <returns>The binary mask image from the threshold.</returns>
	cv::Ptr<cv::Mat> ImgProc_TwoStDevFromMean(cv::Ptr<cv::Mat> src, double& foundThresh);

};