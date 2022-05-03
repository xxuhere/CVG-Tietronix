#include "CamImpl_OpenCVBase.h"

bool CamImpl_OpenCVBase::InitializeImpl()
{ 
	return true;
}

bool CamImpl_OpenCVBase::ShutdownImpl()
{
	this->Deactivate();
	return true;
}

bool CamImpl_OpenCVBase::ActivateImpl()
{
	this->AssertStreamNull();
	this->ocvStream = this->CreateVideoCapture();

	if(this->ocvStream == nullptr)
		return false;

	this->InitCapture();

	return true;
}

bool CamImpl_OpenCVBase::DeactivateImpl()
{
	if(this->ocvStream == nullptr)
		return true;

	if(this->ocvStream->isOpened())
		this->ocvStream->release();
	
	delete this->ocvStream;
	this->ocvStream = nullptr;
	return true;
}

cv::Ptr<cv::Mat> CamImpl_OpenCVBase::PollFrameImpl()
{
	cvgAssert(this->ocvStream != nullptr, "polling with nullstream");

	cv::Ptr<cv::Mat> ret = new cv::Mat();
	*this->ocvStream >> *ret;

	return ret;
}

bool CamImpl_OpenCVBase::InitCapture(cv::VideoCapture* capture)
{
	capture->set(cv::CAP_PROP_BUFFERSIZE, 1);
	capture->set(cv::CAP_PROP_FPS, 30);

	// https://stackoverflow.com/a/69476456/2680066
	capture->set(cv::CAP_PROP_AUTO_EXPOSURE, 1);
	return true;
}

void CamImpl_OpenCVBase::InitCapture()
{
	cvgAssert(this->ocvStream != nullptr, "initializing capture with null stream");
	this->InitCapture(this->ocvStream);
}

bool CamImpl_OpenCVBase::IsValid()
{
	return 
		this->IsStreamAllocated() && 
		this->ocvStream->isOpened();
}