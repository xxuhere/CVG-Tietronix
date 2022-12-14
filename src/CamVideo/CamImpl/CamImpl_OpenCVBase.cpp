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

	this->UtilToFlipMatInOpenCV(*ret);

	return ret;
}

bool CamImpl_OpenCVBase::InitCapture(cv::VideoCapture* capture)
{
	// Keep the buffer as small as possible so while the video
	// will not be expected to be silky smooth, the latency
	// will be as low as possible.
	capture->set(cv::CAP_PROP_BUFFERSIZE, 1);

	// Standardize the FPS rate.
	capture->set(cv::CAP_PROP_FPS, 30);

	if(this->prefWidth != 0)
		capture->set(cv::CAP_PROP_FRAME_WIDTH, this->prefWidth);

	if(this->prefHeight != 0)
		capture->set(cv::CAP_PROP_FRAME_HEIGHT, this->prefHeight);

	// Note that setting the exposure time may not guarantee it's
	// used as requested. It's up to OpenCV and the implementation
	// to (be able to) honor this value.
	if(this->exposureTime != 0)
	{ 
		// Apparently, with CAP_PROP_AUTO_EXPOSURE, 0.25 is some magical number
		// that turns on manual control
		//
		// Although this may be online misinformation as it does not match the
		// else statement's parameter for what is essentially the same thing.
		capture->set(cv::CAP_PROP_AUTO_EXPOSURE, 0.25);

		// And CAP_PROP_EXPOSURE takes in a power of two of the parts per second.
		double seconds = this->exposureTime / 1000000.0; // Microseconds to seconds
		double log2Val = log(seconds) / log(2.0);
		capture->set(cv::CAP_PROP_EXPOSURE, log2Val);
	}
	else
	{ 
		// https://stackoverflow.com/a/69476456/2680066
		// On some webcams, changing the exposure behaviour will
		// allow it to run faster. Note that this may be naive
		// in that the settings we use for one webcam may not apply
		// to all other webcams.
		//
		// Also note that this is being set on a VideoCapture made
		// in the base CamImpl_OpenCVBase class, but only really applies
		// to specific subclasses, mainly ther OCV_USB and OCV_HWPath.
		capture->set(cv::CAP_PROP_AUTO_EXPOSURE, 1);
	}
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

void CamImpl_OpenCVBase::DelegatedInjectIntoDicom(DcmDataset* dicomData)
{
	// TODO: Placeholder, this should be more specific.
	dicomData->putAndInsertString(DCM_SensorName, "OpenCV Stream");
}

bool CamImpl_OpenCVBase::PullOptions(const cvgCamFeedLocs& opts)
{
	this->exposureTime = opts.videoExposureTime;

	return this->ICamImpl::PullOptions(opts);
}