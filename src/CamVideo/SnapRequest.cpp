#include "SnapRequest.h"

SnapRequest::SnapRequest()
{}

bool SnapRequest::Cancel()
{
	if(this->status != Status::Requested)
		return false;

	this->status = Status::Error;
	this->err = "There was a request to cancel.";
	return true;
}

SnapRequest::SPtr SnapRequest::MakeRequest(const std::string& filename, ProcessType processType)
{
	SnapRequest* newReq = new SnapRequest();
	newReq->filename	= filename;
	newReq->frameID		= -1;
	newReq->status		= Status::Unknown;
	newReq->processType = processType;

	return SnapRequest::SPtr(newReq);
}

SnapRequest::SPtr SnapRequest::MakeError(const std::string& err, const std::string& filename)
{
	SnapRequest* newReq = new SnapRequest();
	newReq->filename	= filename;
	newReq->frameID		= -1;
	newReq->status		= Status::Error;
	newReq->err			= err;

	return SnapRequest::SPtr(newReq);
}