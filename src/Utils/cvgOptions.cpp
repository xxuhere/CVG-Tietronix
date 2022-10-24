#include "cvgOptions.h"
#include <istream>
#include <fstream>
#include <iomanip>
#include <iostream>

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
static const char* szKey_fullscreen			= "_fullscreen";		// TODO: Fix variable casing.
static const char* szKey_mousepad_x			= "_mousepad_x";
static const char* szKey_mousepad_y			= "_mousepad_y";
static const char* szKey_mousepad_scale		= "_mousepad_scale";
static const char* szKey_debugUI			= "_debug_ui";
static const char* szKey_compositeWidth		= "composite_width";
static const char* szKey_compositeHeight	= "composite_height";

static const char* szkey_FeedOpts			= "feed_options";
static const char* szKey_CarouselSeries		= "carousel_series";
static const char* szKey_CarouselBody		= "carousel_body";
static const char* szKey_CarouselOrient		= "carousel_orientation";

static const char* szKey_ExposureID			= "exposure_idx";
static const char* szKey_ExposureEntry		= "exposure_entries";
static const char* szKey_ExposureEntryLbl	= "label";
static const char* szKey_ExposureEntryMicro	= "microseconds";

// Try-get utilities for JSON - We could probably get away with
// templating this, but for now we'll just have a few explicit 
// definitions for now to reduce implementation complexity.
bool JSONGetMember(const json& obj, const std::string& memberName, std::string& into)
{
	if(!obj.contains(memberName) || !obj[memberName].is_string())
		return false;

	into = obj[memberName];
}


bool JSONGetMember(const json& obj, const std::string& memberName, int& into)
{
	if(!obj.contains(memberName) || !obj[memberName].is_number())
		return false;

	into = obj[memberName];
}


bool JSONGetMember(const json& obj, const std::string& memberName, float& into)
{
	if(!obj.contains(memberName) || !obj[memberName].is_number())
		return false;

	into = obj[memberName];
}

bool JSONGetMember(const json& obj, const std::string& memberName, long& into)
{
	if(!obj.contains(memberName) || !obj[memberName].is_number())
		return false;

	into = obj[memberName];
}

bool JSONGetMember(const json& obj, const std::string& memberName, bool& into)
{
	if(!obj.contains(memberName) || !obj[memberName].is_boolean())
		return false;

	into = obj[memberName];
}


// Overloads of the above utility functions, but instead of having a failure return,
// they simply set the value to a default if it's missing.
void JSONGetMember(const json& obj, const std::string& memberName, std::string& into, const std::string& def)
{ if(!JSONGetMember(obj, memberName, into)){into = def;}}

void JSONGetMember(const json& obj, const std::string& memberName, int& into, int def)
{ if(!JSONGetMember(obj, memberName, into)){into = def;}}

void JSONGetMember(const json& obj, const std::string& memberName, float& into, float def)
{ if(!JSONGetMember(obj, memberName, into)){into = def;}}

void JSONGetMember(const json& obj, const std::string& memberName, long& into, long def)
{ if(!JSONGetMember(obj, memberName, into)){into = def;}}

void JSONGetMember(const json& obj, const std::string& memberName, bool& into, bool def)
{ if(!JSONGetMember(obj, memberName, into)){into = def;}}


cvgOptions::ExposureSetting::ExposureSetting()
{
	this->label = "";
	this->microseconds = 0;
}

cvgOptions::ExposureSetting::ExposureSetting(const std::string& label, long microseconds)
{
	this->label = label;
	this->microseconds = microseconds;
}

cvgOptions::cvgOptions(int defSources, bool sampleCarousels)
{
	for(int i = 0; i < defSources; ++i)
	{
		// DEFAULT AppOptions.json VALUES:
		//
		// The default two values. These can be removed or overridden, but we 
		// populate it with these default values so that if we save the default
		// cvgOptions into the AppOptions.json, it has the correct expected values.
		cvgCamFeedSource src;
		src.defPoll				= VideoPollType::OpenCVUSB_Idx;
		src.linuxOverRidePoll	= VideoPollType::MMAL;
		src.camIndex			= i;
		src.camMMALIdx			= i;

		// Default image processing settings for menu_targ
		// https://github.com/Achilefu-Lab/CVG-Tietronix/issues/11
		if(i == 0)
		{
			src.menuTarg = true;
			//src.processing = ProcessingType::yen_threshold_compressed;

			// Camera 0 to have 
			// See comment in https://github.com/Achilefu-Lab/CVG-Tietronix/issues/6
			src.flipHorizontal = true; 
		}

		this->feedOpts.push_back(src);
	}

	if(sampleCarousels)
	{ 
		// Hardcode some example carousel items
		//
		// Note that this does mean these sample files, even if we never intend
		// for them to be used during operation, MUST ALWAYS EXIST in case the
		// default carousel items are used.
		this->caroSysSeries.entries.push_back(CarouselData( "CAV",	"Assets/CarIcons/Cavity.png",			"CAV",		"Cavity"));
		this->caroSysSeries.entries.push_back(CarouselData( "XV",	"Assets/CarIcons/ExVivo.png",			"XVVO",		"ExVivo"));
		this->caroSysSeries.entries.push_back(CarouselData( "XVL",	"Assets/CarIcons/ExVivoLymph.png",		"XVLYM",	"ExVivo Lymph"));
		this->caroSysSeries.entries.push_back(CarouselData( "IV",	"Assets/CarIcons/InVivo.png",			"IVVO",		"InVivo"));
		this->caroSysSeries.entries.push_back(CarouselData( "LY",	"Assets/CarIcons/Lymph.png",			"LYM",		"Lymph"));
		this->caroSysSeries.entries.push_back(CarouselData( "SLC",	"Assets/CarIcons/Slices.png",			"SL",		"Slices"));
		this->caroSysSeries.entries.push_back(CarouselData( "SM",	"Assets/CarIcons/ShavedMargin.png",		"SHVM",		"Shaved Margin"));

		this->caroBody.entries.push_back(CarouselData(		"LB",	"Assets/CarIcons/LeftBreast.png",		"LB",		"Left Breast"));
		this->caroBody.entries.push_back(CarouselData(		"RB",	"Assets/CarIcons/RightBreast.png",		"RB",		"Right Breast"));
		this->caroBody.entries.push_back(CarouselData(		"LBR",	"Assets/CarIcons/LeftBreastCav.png",	"LBC",		"Left Breast Cavity"));
		this->caroBody.entries.push_back(CarouselData(		"RBC",	"Assets/CarIcons/RightBreastCav.png",	"RBC",		"Right Breast Cavity"));
		this->caroBody.entries.push_back(CarouselData(		"LN",	"Assets/CarIcons/LymphNode.png",		"LN",		"Lymph Node"));
		this->caroBody.entries.push_back(CarouselData(		"TS",	"Assets/CarIcons/TumorSlice.png",		"TS",		"Tumor Slice"));

		this->caroSysOrient.entries.push_back(CarouselData( "AN",	"Assets/CarIcons/Anterior.png",			"ANTR",		"Anterior"));
		this->caroSysOrient.entries.push_back(CarouselData( "SU",	"Assets/CarIcons/Superior.png",			"SUPR",		"Superior"));
		this->caroSysOrient.entries.push_back(CarouselData( "PO",	"Assets/CarIcons/Posterior.png",		"PSTR",		"Posterior"));
		this->caroSysOrient.entries.push_back(CarouselData( "IN",	"Assets/CarIcons/Inferior.png",			"INFR",		"Inferior"));
		this->caroSysOrient.entries.push_back(CarouselData( "LA",	"Assets/CarIcons/Lateral.png",			"LATR",		"Lateral"));
		this->caroSysOrient.entries.push_back(CarouselData( "ME",	"Assets/CarIcons/Medial.png",			"MEDI",		"Medial"));
	}

	this->SetExposureEntriesToDefault();
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
	JSONGetMember(data, szKey_TestImg,			this->testImagePath);
	JSONGetMember(data, szKey_VPWidth,			this->viewportX);
	JSONGetMember(data, szKey_VPHeight,			this->viewportY);
	JSONGetMember(data, szKey_compositeWidth,	this->compositeWidth);
	JSONGetMember(data, szKey_compositeHeight,	this->compositeHeight);
	JSONGetMember(data, szKey_VPOffsX,			this->viewportOffsX);
	JSONGetMember(data, szKey_VPOffsY,			this->viewportOffsY);
	JSONGetMember(data, szKey_mousepad_x,		this->mousepadX);
	JSONGetMember(data, szKey_mousepad_y,		this->mousepadY);
	JSONGetMember(data, szKey_mousepad_scale,	this->mousepadScale);
	JSONGetMember(data, szKey_debugUI,			this->drawUIDebug);
	JSONGetMember(data, szKey_fullscreen,		this->fullscreen);

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

	if(data.contains(szKey_CarouselBody))
		this->caroBody.ApplyJSON(data[szKey_CarouselBody]);
	
	if(data.contains(szKey_CarouselSeries))
		this->caroSysSeries.ApplyJSON(data[szKey_CarouselSeries]);

	if(data.contains(szKey_CarouselOrient))
		this->caroSysOrient.ApplyJSON(data[szKey_CarouselOrient]);

	this->exposures.clear();
	if(data.contains(szKey_ExposureEntry) && data[szKey_ExposureEntry].is_array())
	{
		const json& jsonExpEnt = data[szKey_ExposureEntry];
		int entryCt = std::min<int>(jsonExpEnt.size(), SUPPORTED_EXPOSURE_ENTRIES);
		for(int i = 0; i < entryCt; ++i)
		{
			ExposureSetting expToAdd;
			JSONGetMember(jsonExpEnt[i], szKey_ExposureEntryLbl, expToAdd.label);
			JSONGetMember(jsonExpEnt[i], szKey_ExposureEntryMicro, expToAdd.microseconds);
			this->exposures.push_back(expToAdd);

			const int EXPOSURE_FRAMERATE_MAX = 34243;
			if( expToAdd.microseconds > EXPOSURE_FRAMERATE_MAX)
			{
				std::cout << "WARNING: Detecting loading exposure option " << i << " with label " << expToAdd.label << " with a microsecond window of " << expToAdd.microseconds <<
					", the max supported by default is " << EXPOSURE_FRAMERATE_MAX << std::endl;
			}
		}
	}
	else
		this->SetExposureEntriesToDefault();

	JSONGetMember(data, szKey_ExposureID, this->defaultExposureID);
}

int cvgOptions::FindMenuTargetIndex() const
{
	for(int i = 0; i < this->feedOpts.size(); ++i)
	{
		if(this->feedOpts[i].menuTarg)
			return i;
	}

	return -1;
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
	ret[szKey_Version			]	= "0.0.0";

	ret[szKey_TestImg			]	= this->testImagePath;
	ret[szKey_VPWidth			]	= this->viewportX;
	ret[szKey_VPHeight			]	= this->viewportY;
	ret[szKey_compositeWidth	]	= this->compositeWidth;
	ret[szKey_compositeHeight	]	= this->compositeHeight;
	ret[szKey_VPOffsX			]	= this->viewportOffsX;
	ret[szKey_VPOffsY			]	= this->viewportOffsY;
	ret[szKey_mousepad_x		]	= this->mousepadX;
	ret[szKey_mousepad_y		]	= this->mousepadY;
	ret[szKey_mousepad_scale	]	= this->mousepadScale;

	ret[szKey_debugUI			]	= this->drawUIDebug;

	ret[szKey_fullscreen		]	= this->fullscreen;

	//		VIDEO FEED ENTRIES
	//////////////////////////////////////////////////
	json feedOpts = json::array();
	for(const cvgCamFeedSource& feedSrc: this->feedOpts)
		feedOpts.push_back(feedSrc.AsJSON());
	
	ret[szkey_FeedOpts] = feedOpts;

	//		CAROUSEL ENTRIES
	//////////////////////////////////////////////////
	ret[szKey_CarouselBody]		= this->caroBody.AsJSON();
	ret[szKey_CarouselSeries]	= this->caroSysSeries.AsJSON();
	ret[szKey_CarouselOrient]	= this->caroSysOrient.AsJSON();

	//		EXPOSURES
	//////////////////////////////////////////////////
	ret[szKey_ExposureID] = this->defaultExposureID;
	int expEntCt = std::min<int>(this->exposures.size(), SUPPORTED_EXPOSURE_ENTRIES);
	json jsonExpEntries = json::array();
	for(int i = 0; i < expEntCt; ++i)
	{
		json jsonEntry = json::object();
		jsonEntry[szKey_ExposureEntryLbl] = this->exposures[i].label;
		jsonEntry[szKey_ExposureEntryMicro] = this->exposures[i].microseconds;
		//
		jsonExpEntries.push_back(jsonEntry);
	}
	ret[szKey_ExposureEntry] = jsonExpEntries;

	return ret;
}

void cvgOptions::Clear()
{
	// Take default values from default constructor.
	*this = cvgOptions(0, false);
}

void cvgOptions::SetExposureEntriesToDefault()
{
	this->exposures.clear();
	//
	this->exposures.push_back(ExposureSetting( "Auto", 0));
	this->exposures.push_back(ExposureSetting( "10ms", 10000));
	this->exposures.push_back(ExposureSetting( "22ms", 32000));
	this->exposures.push_back(ExposureSetting( "33ms", 33000));
}

cvgOptions::ExposureSetting cvgOptions::GetExposureEntry(int index) const
{
	if(index < 0 || index >= this->exposures.size())
		return ExposureSetting(); // If not valid, return a default

	return this->exposures[index];
}