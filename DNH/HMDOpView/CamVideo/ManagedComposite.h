#pragma once

#include "IManagedCam.h"
#include <map>

class ManagedComposite : public IManagedCam
{
private:
	static bool cacheAvailable;
	static std::map<int, cv::Ptr<cv::Mat>> globalCache;
	static std::mutex cacheMutex;
	static bool modSinceLastCache;

private:

protected:
	void _EndShutdown() override;

public:

	static bool CacheCameraFrame(int id, cv::Ptr<cv::Mat> img);

	cv::Ptr<cv::Mat> ProcessImage(cv::Ptr<cv::Mat> inImg) override;

	float GetFloat( StreamParams paramid) override;

	bool SetFloat( StreamParams paramid, float value) override;

	bool UsesImageProcessingChain() override;

	virtual void ThreadFn(int camIdx) override;

	virtual CamType GetCamType() override;

	virtual int GetID() const override;

	virtual std::string GetStreamName() const override;

};