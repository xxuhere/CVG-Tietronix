#include "CarouselData.h"
#include <fstream>

const char * szJSKey_ID			= "id";
const char * szJSKey_IcoFile	= "icon_filepath";
const char * szJSKey_Label		= "label";
const char * szJSKey_Caption	= "caption";

//
static const char* szKey_SystemEntries = "entries";


CarouselData::CarouselData()
{}

CarouselData::CarouselData(
	const std::string& id, 
	const std::string& iconFilepath,
	const std::string& label,
	const std::string& caption)
{
	this->id			= id;
	this->iconFilepath	= iconFilepath;
	this->label			= label;
	this->caption		= caption;
}

json CarouselData::AsJSON() const
{
	json ret = json::object();
	ret[szJSKey_ID		]	= this->id;
	ret[szJSKey_IcoFile	]	= this->iconFilepath;
	ret[szJSKey_Label	]	= this->label;
	ret[szJSKey_Caption	]	= this->caption;
	return ret;
}

bool CarouselData::ApplyJSON(const json& js)
{
	bool allParsed = true;

	if(js.contains(szJSKey_ID) && js[szJSKey_ID].is_string())
		this->id = js[szJSKey_ID];
	else
		allParsed = false;

	if(js.contains(szJSKey_IcoFile) && js[szJSKey_IcoFile].is_string())
		this->iconFilepath = js[szJSKey_IcoFile];
	else
		allParsed = false;

	if(js.contains(szJSKey_Label) && js[szJSKey_Label].is_string())
		this->label = js[szJSKey_Label];
	else
		allParsed = false;

	if(js.contains(szJSKey_Caption) && js[szJSKey_Caption].is_string())
		this->caption = js[szJSKey_Caption];
	else
		allParsed = false;

	return allParsed;
}

void CarouselSystemData::Clear()
{
	this->entries.clear();
}

bool CarouselSystemData::ApplyJSON(const json& js)
{
	if(js.is_array())
	{
		// If array, use CarouselData to load and store
		// entry data.
		for(const json& jsCarOpt : js)
		{
			if(!jsCarOpt.is_object())
				continue;

			CarouselData cdData;
			if(!cdData.ApplyJSON(jsCarOpt)) // TODO: better error notification
				continue;

			this->entries.push_back(cdData);
		}
		return true;
	}
	else if(js.is_string())
	{
		// If string, use that as a path that contains our
		// data and load that file.
		std::string fileToRef = js;
		std::ifstream ifs(fileToRef);
		if(!ifs.is_open())
			return false;

		try
		{ 
			json loaded = json::parse(ifs);
			if(!loaded.contains(szKey_SystemEntries))
				return false;

			// The JSON file format will be pretty much the same, so we'll
			// just create a temporary CarouselSystemData to load it for us -
			// and then we'll just steal the results.
			CarouselSystemData loadingHelper;
			if(loadingHelper.ApplyJSON(loaded[szKey_SystemEntries]))
			{ 
				this->AddCopyCatEntries(loadingHelper);
				return true;
			}
			return false;
		}
		catch(std::exception& /*ex*/)
		{
			return false;
		}
	}
	return false;
}

json CarouselSystemData::AsJSON() const
{
	json jsRet = json::array();
	for(const CarouselData& car: this->entries)
		jsRet.push_back(car.AsJSON());

	return jsRet;
}

void CarouselSystemData::AddCopyCatEntries(CarouselSystemData& csdSrc)
{
	this->entries.insert(
		this->entries.end(),
		csdSrc.entries.begin(),
		csdSrc.entries.end());
}