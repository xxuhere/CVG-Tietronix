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

static std::string _SanitizePrefix(const std::string& str)
{
	return str;
}

std::string OpSession::GenerateSessionPrefix() const
{
	if(this->sessionName.empty())
		return _SanitizePrefix(this->sessionName);

	if(this->patNameLast.empty())
	{
		if(!this->patNameFirst.empty())
		{
			return _SanitizePrefix(this->patNameLast + "_" + this->patNameFirst);
		}
		return _SanitizePrefix(this->patNameLast);
	}
	return "_invalid_";
}