#include "SnapRequest.h"

SnapRequest::SnapRequest()
{}

SnapRequest::SPtr SnapRequest::MakeRequest(const std::string& filename)
{
	SnapRequest* newReq = new SnapRequest();
	newReq->filename = filename;
	newReq->frameID = -1;
	newReq->status = Status::Unknown;

	return SnapRequest::SPtr(newReq);
}