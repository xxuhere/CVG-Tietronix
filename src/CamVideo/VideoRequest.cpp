#include "VideoRequest.h"

VideoRequest::VideoRequest(
	int width, 
	int height, 
	int camId, 
	const std::string& filename)
{
	this->width		= width;
	this->height	= height;
	this->camId		= camId;
	this->filename	= filename;
}

VideoRequest::SPtr VideoRequest::MakeError(const std::string& err)
{
	VideoRequest* newReq = new VideoRequest(-1,-1, 0, "");
	newReq->err = err;
	return VideoRequest::SPtr(newReq);
}

VideoRequest::SPtr VideoRequest::MakeRequest(
	int width, 
	int height, 
	int camId, 
	const std::string& filename)
{
	VideoRequest* newReq = new VideoRequest(width, height, camId, filename);
	return VideoRequest::SPtr(newReq);
}