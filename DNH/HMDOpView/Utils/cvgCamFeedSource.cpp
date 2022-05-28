#include "cvgCamFeedSource.h"
#include <iostream>
#include "multiplatform.h"



json cvgCamFeedSource::AsJSON() const
{
	json ret = json::object();

	ret["default_poll"	] = to_string(this->defPoll);

	if(this->windowsOverRidePoll.has_value())
		ret["windows_poll"	] = to_string(this->windowsOverRidePoll.value());
	if(this->linuxOverRidePoll.has_value())
		ret["linux_poll"] = to_string(this->linuxOverRidePoll.value());

	ret["index"			] = this->camIndex;
	ret["uri"			] = this->uriSource;
	ret["dev_path"		] = this->devicePath;
	ret["pipe_cmd"		] = this->externalPipeCmd;
	ret["pipe_chans"	] = this->channelCtFromPipe;
	ret["pipe_width"	] = this->pipeWidth;
	ret["pipe_height"	] = this->pipeHeight;
	ret["static_img"	] = this->staticImagePath;

	if(this->processing == ProcessingType::static_threshold)
		ret["processing"] = this->thresholdExplicit;
	else
		ret["processing"	] = to_string(this->processing);

	return ret;
}

void cvgCamFeedSource::ApplyJSON(const json& js)
{
	//Default poll, might be replaced by other specific defaults
	if (js.contains("default_poll") && js["default_poll"].is_string())
		this->defPoll = StringToPollType(js["default_poll"]);

	if (js.contains("linux_poll") && js["linux_poll"].is_string())
		this->linuxOverRidePoll = StringToPollType(js["linux_poll"]);

	if (js.contains("windows_poll") && js["windows_poll"].is_string())
		this->windowsOverRidePoll = StringToPollType(js["windows_poll"]);

	if(js.contains("index") && js["index"].is_number())
		this->camIndex = js["index"];

	if(js.contains("uri") && js["uri"].is_string())
		this->uriSource = js["uri"];

	if(js.contains("dev_path") && js["dev_path"].is_string())
		this->devicePath = js["dev_path"];

	if(js.contains("pipe_cmd") && js["pipe_cmd"].is_string())
		this->externalPipeCmd = js["pipe_cmd"];

	if(js.contains("pipe_chans") && js["pipe_chans"].is_string())
		this->channelCtFromPipe = js["pipe_chans"];

	if(js.contains("pipe_width") && js["pipe_width"].is_number())
		this->pipeWidth = js["pipe_width"];

	if(js.contains("pipe_height") && js["pipe_height"].is_number())
		this->pipeHeight = js["pipe_height"];

	if(js.contains("static_img") && js["static_img"].is_string())
		this->staticImagePath = js["static_img"];

	if (js.contains("processing"))
	{
		if(js["processing"].is_string())
			this->processing = StringToProcessingType(js["processing"]);
		else if(js["processing"].is_number()) // Arguable should be is_number_integer()
		{
			this->processing = ProcessingType::static_threshold;
			this->thresholdExplicit = js["processing"];
		}
	}
}

VideoPollType cvgCamFeedSource::GetUsedPoll() const
{
	VideoPollType ret = this->defPoll;

#if IS_RPI
	if(this->linuxOverRidePoll.has_value())
		ret = this->linuxOverRidePoll.value();
#elif _WIN32
	if(this->windowsOverRidePoll.has_value())
		ret = this->windowsOverRidePoll.value();
#endif

	return ret;
}