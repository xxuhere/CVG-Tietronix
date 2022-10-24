#include "ICamImpl.h"
#include "../../Utils/cvgOptions.h"

bool ICamImpl::Initialize()
{
	if(this->hasInitialized)
		return false;

	this->hasInitialized = this->InitializeImpl();
	return this->hasInitialized;
}

bool ICamImpl::Shutdown()
{
	if(!this->hasInitialized || this->hasShutdown)
		return false;

	bool ret = this->ShutdownImpl();
	this->hasShutdown = true;

	return ret;
}

bool ICamImpl::Activate()
{
	// For now it's just a direct delegation to the 
	// subclass implementation.
	return this->ActivateImpl();
}

bool ICamImpl::Deactivate()
{
	// For now it's just a direct delegation to the 
	// subclass implementation.
	return this->DeactivateImpl();
}

ICamImpl::~ICamImpl()
{
	this->Shutdown();
}

bool ICamImpl::PullOptions(const cvgCamFeedLocs& opts)
{
	this->prefWidth		= opts.streamWidth;
	this->prefHeight	= opts.streamHeight;
	this->flipHoriz		= opts.flipHorizontal;
	this->flipVert		= opts.flipVertical;
	return true;
}

void ICamImpl::DelegatedInjectIntoDicom(DcmDataset* dicomData)
{
	// Does nothing, subclasses are expected to implement this.
}

void ICamImpl::UtilToFlipMatInOpenCV(cv::Mat& mat)
{
	// https://docs.opencv.org/3.4/d2/de8/group__core__array.html#gaca7be533e3dac7feb70fc60635adf441
	if(this->flipHoriz && this->flipVert)
		cv::flip(mat, mat, -1);
	else if(this->flipHoriz)
		cv::flip(mat, mat, 1);
	else if(this->flipVert)
		cv::flip(mat, mat, 0);
}

bool ICamImpl::SetParam(StreamParams paramid, double value)
{
	std::cout << "Unhandled parameter " << paramid << "In ICamImpl::SetParam()" << std::endl;
	return false;
}