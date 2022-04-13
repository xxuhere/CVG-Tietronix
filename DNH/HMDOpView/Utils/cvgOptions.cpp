#include "cvgOptions.h"
#include <istream>
#include <fstream>
#include <iomanip>

cvgOptions::cvgOptions()
{}

bool cvgOptions::LoadFromFile(const std::string& filepath)
{
	std::ifstream ifs(filepath);
	if(!ifs.is_open())
		return false;

	try
	{ 
		json loaded = json::parse(ifs);
		this->Clear();
		this->Apply(loaded);
	}
	catch(std::exception& ex)
	{
		return false;
	}
	return true;
}

bool cvgOptions::SaveToFile(const std::string& filepath) const
{
	std::ofstream ofs(filepath);
	if(!ofs.is_open())
		return false;

	json jsRepr = this->RepresentAsJSON();

	// Printing a beautified json output with nhloman
	// https://stackoverflow.com/q/47834320/2680066
	//
	// I don't think the std::endl is required for its newline, 
	// but to finalize flushing the buffer.
	ofs << std::setw(4) << jsRepr << std::endl;
	return true;
}

void cvgOptions::Apply(json& data)
{
	const char * szKey_TestImg = "_test_img_path";
	if(data.contains(szKey_TestImg) && data[szKey_TestImg].is_string())
		this->testImagePath = data[szKey_TestImg];

	const char * szKey_VPWidth = "_viewport_width";
	if(data.contains(szKey_VPWidth) && data[szKey_VPWidth].is_number())
		this->viewportX = data[szKey_VPWidth];

	const char * szKey_VPHeight = "_viewport_height";
	if(data.contains(szKey_VPHeight) && data[szKey_VPHeight].is_number())
		this->viewportY = data[szKey_VPHeight];
}

json cvgOptions::RepresentAsJSON() const
{
	// Developer tooling values should follow the convention
	// of starting with an underscore ('_').
	//
	// The convention used for variables here is decided to be
	// all lower-case, with worse separated by underscores.
	//
	json ret = json::object();
	ret["_test_img_path"	] = this->testImagePath;
	ret["_viewport_width"	] = this->viewportX;
	ret["_viewport_height"	] = this->viewportY;
	return ret;
}

void cvgOptions::Clear()
{
	// Take default values from default constructor.
	*this = cvgOptions();
}