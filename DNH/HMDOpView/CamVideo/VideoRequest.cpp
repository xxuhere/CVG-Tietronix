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

VideoRequest::SPtr VideoRequest::MakeRequest(
	int width, 
	int height, 
	int camId, 
	const std::string& filename)
{
	VideoRequest* newReq = new VideoRequest(width, height, camId, filename);
	return VideoRequest::SPtr(newReq);
}