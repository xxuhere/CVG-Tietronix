#pragma once

#include "ICamImpl.h"
#include <opencv2/videoio.hpp>

class CamImpl_OpenCVBase : public ICamImpl
{
private:
	cv::VideoCapture* ocvStream = nullptr;

protected:
	bool InitializeImpl() override;
	bool ShutdownImpl() override;
	bool ActivateImpl() override;
	bool DeactivateImpl() override;
	cv::Ptr<cv::Mat> PollFrameImpl() override;

	virtual cv::VideoCapture * CreateVideoCapture() = 0;
	virtual bool InitCapture(cv::VideoCapture* capture);
	void InitCapture();

	inline bool IsStreamAllocated()
	{ return this->ocvStream != nullptr; }

	inline void AssertStreamNull()
	{ assert(!this->IsStreamAllocated()); }

public:
	// No poll type here, that's the responsibility for 
	// subclasses (that use OpenCV) to specify.
	//
	//VideoPollType PollType() override;
	bool IsValid() override;
};