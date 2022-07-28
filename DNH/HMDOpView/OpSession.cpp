#include "OpSession.h"

#include <istream>
#include <fstream>
#include <iomanip>

static const char* szKey_Firstname			= "firstname";
static const char* szKey_Middlename			= "middlename";
static const char* szKey_Lastname			= "lastname";
static const char* szKey_Sessionname		= "session";

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
	if(!this->sessionName.empty())
		return _SanitizePrefix(this->sessionName);

	if(!this->patNameLast.empty())
	{
		if(!this->patNameFirst.empty())
		{
			return _SanitizePrefix(this->patNameLast + "_" + this->patNameFirst);
		}
		return _SanitizePrefix(this->patNameLast);
	}
	return "_invalid_";
}

json OpSession::RepresentationAsJSON() const
{
	json ret = json::object();
	ret[szKey_Firstname		] = this->patNameFirst;
	ret[szKey_Middlename	] = this->patNameMid;
	ret[szKey_Lastname		] = this->patNameLast;
	ret[szKey_Sessionname	] = this->sessionName;
	return ret;
}

void OpSession::Apply(json& data)
{
	if(data.contains(szKey_Firstname) && data[szKey_Firstname].is_string())
		this->patNameFirst = data[szKey_Firstname];

	if(data.contains(szKey_Middlename) && data[szKey_Middlename].is_string())
		this->patNameMid = data[szKey_Middlename];

	if(data.contains(szKey_Lastname) && data[szKey_Lastname].is_string())
		this->patNameLast = data[szKey_Lastname];

	if(data.contains(szKey_Sessionname) && data[szKey_Sessionname].is_string())
		this->sessionName = data[szKey_Sessionname];
}

bool OpSession::LoadFromFile(const std::string& filepath)
{
	std::ifstream ifs(filepath);
	if(!ifs.is_open())
		return false;

	try
	{ 
		json loaded = json::parse(ifs);
		this->Default();
		this->Apply(loaded);
	}
	catch(std::exception& /*ex*/)
	{
		return false;
	}
	return true;
}

bool OpSession::SaveToFile(const std::string& filepath) const
{
	std::ofstream ofs(filepath);
	if(!ofs.is_open())
		return false;

	json jsRepr = this->RepresentationAsJSON();

	// Printing a beautified json output with nhloman
	// https://stackoverflow.com/q/47834320/2680066
	//
	// I don't think the std::endl is required for its newline, 
	// but to finalize flushing the buffer.
	ofs << std::setw(4) << jsRepr << std::endl;
	return true;
}

void OpSession::Clear()
{
	this->patNameFirst	= "";
	this->patNameMid	= "";
	this->patNameLast	= "";
	this->sessionName	= "";
}

void OpSession::Default()
{
	// Use the default constructor and transfer those default
	// member values to ourself.
	*this = OpSession();
}