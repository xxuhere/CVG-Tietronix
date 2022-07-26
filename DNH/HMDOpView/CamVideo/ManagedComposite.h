#pragma once

#include "IManagedCam.h"
#include <map>

/// <summary>
/// Structure to cache image for compositing, with extra
/// image info.
/// </summary>
struct CompCacheInfo
{
public:
	/// <summary>
	/// The cached image.
	/// </summary>
	cv::Ptr<cv::Mat> img;

	/// <summary>
	/// If the image a threshold image? 
	/// 
	/// This is important when rendering a single component image. While RGB
	/// images are already reddened, there's ambiguity on whether a 1 comp
	/// image should be a greyscale or a red image.
	/// </summary>
	bool thresholded;

	/// <summary>
	/// May only be relevant for thresholded images. Determines
	/// the additive blending for the compositing.
	/// </summary>
	float opacity = 1.0f;

public:
	CompCacheInfo();
	CompCacheInfo(cv::Ptr<cv::Mat> img, bool thresholded, float opacity);
};

/// <summary>
/// IManagedCam subclass implementation for suporting composited video.
/// </summary>
class ManagedComposite : public IManagedCam
{
private:
	
	// These static variables are essentially global singleton
	// variables - we only expect there to be (at most) 1 ManagedComposite
	// active during the lifetime of the application.

	/// <summary>
	/// Specifies whether globalCache is available for use.
	/// </summary>
	static bool cacheAvailable;

	/// <summary>
	/// The cache of the last known images of video feeds to composite.
	/// </summary>
	static std::map<int, CompCacheInfo> globalCache;

	/// <summary>
	/// Thread protection for globalCache.
	/// </summary>
	static std::mutex cacheMutex;

	/// <summary>
	/// A dirty flag to record if any of the contents of globalCache have
	/// been modified since the last composite was generated.
	/// </summary>
	static bool modSinceLastCache;

private:

protected:
	void _EndShutdown() override;

public:

	/// <summary>
	/// The interface for other IManagedCam object to submit their
	/// current frames to be queued for compositing.
	/// </summary>
	static bool CacheCameraFrame(int id, cv::Ptr<cv::Mat> img, bool thresholded, float opacity);

	cv::Ptr<cv::Mat> ProcessImage(cv::Ptr<cv::Mat> inImg) override;

	float GetFloat( StreamParams paramid) override;

	bool SetFloat( StreamParams paramid, float value) override;

	bool UsesImageProcessingChain() override;

	virtual void ThreadFn(int camIdx) override;

	virtual CamType GetCamType() override;

	virtual int GetID() const override;

	virtual std::string GetStreamName() const override;

};