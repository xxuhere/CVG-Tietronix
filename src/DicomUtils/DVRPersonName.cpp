#include "DVRPersonName.h"

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>

DVRPersonName::DVRPersonName()
{}

DVRPersonName::DVRPersonName(const std::string& dicomForm)
{
	this->LoadFromDicomForm(dicomForm);
}

DVRPersonName::DVRPersonName(
	const std::string& last, 
	const std::string& middle, 
	const std::string& first,
	const std::string& prefix,
	const std::string& suffix)
{
	this->Set(
		last,
		middle,
		first,
		prefix,
		suffix);
}

void DVRPersonName::Set(
	const std::string& last, 
	const std::string& middle, 
	const std::string& first,
	const std::string& prefix,
	const std::string& suffix)
{
	this->lastName		= last;
	this->middleName	= middle;
	this->firstName		= first;
	this->prefix		= prefix;
	this->suffix		= suffix;
}

// Excerpt from from the spec:
//
//	"For human use, the five components in their order 
//	of occurrence are: family name complex, given name 
//	complex, middle name, name prefix, name suffix."
//
//	"Any of the five components may be an empty string. 
//	The component delimiter shall be the caret "^" 
//	character (5EH). There shall be no more than four 
//	component delimiters, i.e., none after the last 
//	component if all components are present. Delimiters 
//	are required for interior null components."
bool DVRPersonName::LoadFromDicomForm(const std::string& str)
{
	std::vector<std::string> splitRes;
	boost::split(splitRes, str, boost::is_any_of("^"));
	
	for(int i = 0; i < splitRes.size(); ++i)
	{
		if(i >= 5)
			break;

		std::string tok = splitRes[i];
		boost::trim(tok);
		if(tok.empty())
			continue;

		switch(i)
		{
		case 0:
			this->lastName = tok;
			break;

		case 1:
			this->middleName = tok;
			break;

		case 2:
			this->firstName = tok;
			break;

		case 3:
			this->prefix = tok;
			break;

		case 4:
			this->suffix = tok;
			break;
		}
	}
	return true;
}

std::string DVRPersonName::ToDicomForm() const
{
	return 
		this->lastName		+ "^" + 
		this->middleName	+ "^" + 
		this->firstName		+ "^" + 
		this->prefix		+ "^" + 
		this->suffix;
}

void DVRPersonName::Clear()
{
	this->lastName.clear();
	this->middleName.clear();
	this->firstName.clear();
	this->prefix.clear();
	this->suffix.clear();
}