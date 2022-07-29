#include "CarouselData.h"

const char * szJSKey_ID			= "id";
const char * szJSKey_IcoFile	= "icon_filepath";
const char * szJSKey_Label		= "label";
const char * szJSKey_Caption	= "caption";

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