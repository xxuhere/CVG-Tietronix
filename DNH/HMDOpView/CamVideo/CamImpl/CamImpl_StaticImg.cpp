#include "CamImpl_StaticImg.h"
#include <boost/filesystem.hpp>
#include <opencv2/imgcodecs.hpp>

CamImpl_StaticImg::CamImpl_StaticImg(const std::string& imgPath)
{
	this->imgPath = imgPath;
}

bool CamImpl_StaticImg::InitializeImpl()
{
	return true;
}

bool CamImpl_StaticImg::ShutdownImpl()
{
	this->DeactivateImpl();
	return true;
}

bool CamImpl_StaticImg::ActivateImpl()
{
	return this->CheckImgPathExists();
}

bool CamImpl_StaticImg::DeactivateImpl()
{
	return true;
}

cv::Ptr<cv::Mat> CamImpl_StaticImg::PollFrameImpl()
{
	cv::Ptr<cv::Mat> ret = new cv::Mat();
	*ret = cv::imread(this->imgPath);

	return ret;
}

bool CamImpl_StaticImg::CheckImgPathExists() const
{
	return boost::filesystem::exists(this->imgPath);
}

VideoPollType CamImpl_StaticImg::PollType()
{
	return VideoPollType::Image;
}

bool CamImpl_StaticImg::IsValid()
{
	return this->CheckImgPathExists();
}

bool CamImpl_StaticImg::PullOptions(const cvgCamFeedLocs& opts)
{
	this->ICamImpl::PullOptions(opts);

	this->imgPath = opts.staticImagePath;
	return true;
}