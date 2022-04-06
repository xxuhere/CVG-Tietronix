#include "OpSession.h"

void OpSession::SetName(
	const std::string& first, 
	const std::string& mid, 
	const std::string& last)
{
	this->patNameFirst	= first;
	this->patNameMid	= mid;
	this->patNameLast	= last;
}

void OpSession::SetSession(const std::string& session)
{
	this->sessionName = session;
}