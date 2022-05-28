#include "cvgOptions.h"
#include <istream>
#include <fstream>
#include <iomanip>

// The JSON keys. This listing is the authority on what literal
// strings can be expected throughout the options json file.
//
// When Adding items, remember to check these things:
// 1) Is the JSON key ONLY defined here?
// 2) Is loading/parsing implemented for the key in cvgOptions::Apply()?
// 3) Is saving implement for the key in cvgOptions::RepresentAsJSON()?
// 4) Is the option being used by the application as appropriate?
//
static const char* szKey_Comment			= "__comment__";
static const char* szKey_Version			= "VERSION";
static const char* szKey_TestImg			= "_test_img_path";
static const char* szKey_VPWidth			= "_viewport_width";
static const char* szKey_VPHeight			= "_viewport_height";
static const char* szKey_VPOffsX			= "_viewport_offsx";
static const char* szKey_VPOffsY			= "_viewport_offsy";
static const char* szKey_fullscreen			= "_fullscreen";
static const char* szKey_mousepad_x			= "_mousepad_x";
static const char* szKey_mousepad_y			= "_mousepad_y";
static const char* szKey_mousepad_scale		= "_mousepad_scale";
static const char* szKey_debugUI			= "_debug_ui";

static const char* szkey_FeedOpts			= "feed_options";
static const char* szKey_Carousel			= "carousel_options";

cvgOptions::cvgOptions(int defSources, bool sampleCarousels)
{
	for(int i = 0; i < defSources; ++i)
	{
		cvgCamFeedSource src;
		src.defPoll = VideoPollType::OpenCVUSB_Idx;
		src.camIndex = i;
		this->feedOpts.push_back(src);
	}

	if(sampleCarousels)
	{ 
		// Hardcode some example carousel items
		//
		// Note that this does mean these sample files, even if we never intend
		// for them to be used during operation, MUST ALWAYS EXIST in case the
		// default carousel items are used.
		this->carouselEntries.push_back(CarouselData( "E1", "Assets/CarIcons/CarIco_Sample1.png", "SAM1", "Sample 1"));
		this->carouselEntries.push_back(CarouselData( "E2", "Assets/CarIcons/CarIco_Sample2.png", "SAM2", "Sample 2"));
		this->carouselEntries.push_back(CarouselData( "E3", "Assets/CarIcons/CarIco_Sample3.png", "SAM3", "Sample 3"));
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
	if(data.contains(szKey_TestImg) && data[szKey_TestImg].is_string())
		this->testImagePath = data[szKey_TestImg];

	if(data.contains(szKey_VPWidth) && data[szKey_VPWidth].is_number())
		this->viewportX = data[szKey_VPWidth];

	if(data.contains(szKey_VPHeight) && data[szKey_VPHeight].is_number())
		this->viewportY = data[szKey_VPHeight];

	if(data.contains(szKey_VPOffsX) && data[szKey_VPOffsX].is_number())
		this->viewportOffsX = data[szKey_VPOffsX];

	if(data.contains(szKey_VPOffsY) && data[szKey_VPOffsY].is_number())
		this->viewportOffsY = data[szKey_VPOffsY];

	if(data.contains(szKey_mousepad_x) && data[szKey_mousepad_x].is_number())
		this->mousepadX = data[szKey_mousepad_x];

	if(data.contains(szKey_mousepad_y) && data[szKey_mousepad_y].is_number())
		this->mousepadY = data[szKey_mousepad_y];

	if(data.contains(szKey_mousepad_scale) && data[szKey_mousepad_scale].is_number())
		this->mousepadScale = data[szKey_mousepad_scale];

	if(data.contains(szKey_debugUI) && data[szKey_debugUI].is_boolean())
		this->drawUIDebug = data[szKey_debugUI];

	if (data.contains(szKey_fullscreen) && data[szKey_fullscreen].is_boolean())
		this->fullscreen = data[szKey_fullscreen];

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

	if(data.contains(szKey_Carousel) && data[szKey_Carousel].is_array())
	{
		for(const json& jsCarOpt : data[szKey_Carousel])
		{
			if(!jsCarOpt.is_object())
				continue;

			CarouselData cdData;
			if(!cdData.ApplyJSON(jsCarOpt)) // TODO: better error notification
				continue;

			this->carouselEntries.push_back(cdData);
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
	ret[szKey_Comment	]		= "Application data used by the HMDOp application. See cvgOptions.h and cvgCamFeedSource.h for more info.";

	//		DIRECT DOCUMENT ROOT ELEMENTS
	//////////////////////////////////////////////////

	// Semantic versioning, so we have to option to check reverse
	// compatibility with these option files in the future.
	ret[szKey_Version	]		= "0.0.0";

	ret[szKey_TestImg	]		= this->testImagePath;
	ret[szKey_VPWidth	]		= this->viewportX;
	ret[szKey_VPHeight	]		= this->viewportY;
	ret[szKey_VPOffsX   ]		= this->viewportOffsX;
	ret[szKey_VPOffsY   ]		= this->viewportOffsY;
	ret[szKey_mousepad_x]		= this->mousepadX;
	ret[szKey_mousepad_y]		= this->mousepadY;
	ret[szKey_mousepad_scale]	= this->mousepadScale;

	ret[szKey_debugUI]			= this->drawUIDebug;

	ret[szKey_fullscreen]		= this->fullscreen;

	//		VIDEO FEED ENTRIES
	//////////////////////////////////////////////////
	json feedOpts = json::array();
	for(const cvgCamFeedSource& feedSrc: this->feedOpts)
		feedOpts.push_back(feedSrc.AsJSON());
	
	ret[szkey_FeedOpts] = feedOpts;

	//		CAROUSEL ENTRIES
	//////////////////////////////////////////////////
	json carouselOpts = json::array();
	for(const CarouselData& car: this->carouselEntries)
		carouselOpts.push_back(car.AsJSON());

	ret[szKey_Carousel] = carouselOpts;

	return ret;
}

void cvgOptions::Clear()
{
	// Take default values from default constructor.
	*this = cvgOptions(0, false);
}