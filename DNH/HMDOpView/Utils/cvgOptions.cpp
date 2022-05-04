#include "cvgOptions.h"
#include <istream>
#include <fstream>
#include <iomanip>

cvgOptions::cvgOptions(int defSources)
{
	for(int i = 0; i < defSources; ++i)
	{
		cvgCamFeedSource src;
		src.defPoll = VideoPollType::OpenCVUSB_Idx;
		src.camIndex = i;
		this->feedOpts.push_back(src);
	}
}

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
	catch(std::exception& /*ex*/)
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
	const char* szKey_TestImg = "_test_img_path";
	if(data.contains(szKey_TestImg) && data[szKey_TestImg].is_string())
		this->testImagePath = data[szKey_TestImg];

	const char* szKey_VPWidth = "_viewport_width";
	if(data.contains(szKey_VPWidth) && data[szKey_VPWidth].is_number())
		this->viewportX = data[szKey_VPWidth];

	const char* szKey_VPHeight = "_viewport_height";
	if(data.contains(szKey_VPHeight) && data[szKey_VPHeight].is_number())
		this->viewportY = data[szKey_VPHeight];

	const char* szKey_VPOffsX = "_viewport_offsx";
	if(data.contains(szKey_VPOffsX) && data[szKey_VPOffsX].is_number())
		this->viewportOffsX = data[szKey_VPOffsX];

	const char* szKey_VPOffsY = "_viewport_offsy";
	if(data.contains(szKey_VPOffsY) && data[szKey_VPOffsY].is_number())
		this->viewportOffsY = data[szKey_VPOffsY];

	const char* szKey_fullscreen = "_fullscreen";
	if (data.contains(szKey_fullscreen) && data[szKey_fullscreen].is_boolean())
		this->fullscreen = data[szKey_fullscreen];

	const char*szkey_FeedOpts = "feed_options";
	if(data.contains(szkey_FeedOpts) && data[szkey_FeedOpts].is_array())
	{
		for(const json& jsFeedOpt : data[szkey_FeedOpts])
		{
			if(!jsFeedOpt.is_object())
				continue;

			cvgCamFeedSource src;
			src.ApplyJSON(jsFeedOpt);
			this->feedOpts.push_back(src);
		}
	}
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
	ret["__comment__"		] = "Application data used by the HMDOp application. See cvgOptions.h and cvgCamFeedSource.h for more info.";

	// Semantic versioning, so we have to option to check reverse
	// compatibility with these option files in the future.
	ret["VERSION"			] = "0.0.0";

	ret["_test_img_path"	] = this->testImagePath;
	ret["_viewport_width"	] = this->viewportX;
	ret["_viewport_height"	] = this->viewportY;
	ret["_viewport_offsx"   ] = this->viewportOffsX;
	ret["_viewport_offsy"   ] = this->viewportOffsY;
	ret["_fullscreen"]		  = this->fullscreen;

	json feedOpts = json::array();
	for(const cvgCamFeedSource& feedSrc: this->feedOpts)
		feedOpts.push_back(feedSrc.AsJSON());
	
	ret["feed_options"] = feedOpts;


	return ret;
}

void cvgOptions::Clear()
{
	// Take default values from default constructor.
	*this = cvgOptions(0);
}