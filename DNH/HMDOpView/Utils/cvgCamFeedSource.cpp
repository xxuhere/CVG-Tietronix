#include "cvgCamFeedSource.h"

json cvgCamFeedSource::AsJSON() const
{
	json ret = json::object();

	ret["default_poll"	] = to_string(this->defPoll);
	ret["windows_poll"	] = to_string(this->windowsDefPoll);
	ret["linux_poll"	] = to_string(this->linuxDefPoll);
	ret["index"			] = this->camIndex;
	ret["uri"			] = this->uriSource;
	ret["dev_path"		] = this->devicePath;
	ret["pipe_cmd"		] = this->externalPipeCmd;
	ret["pipe_chans"	] = this->channelCtFromPipe;
	ret["pipe_width"	] = this->pipeWidth;
	ret["pipe_height"	] = this->pipeHeight;
	ret["static_img"	] = this->staticImagePath;
	ret["processing"	] = to_string(this->processing);

	return ret;
}

void cvgCamFeedSource::ApplyJSON(const json& js)
{
	//Default poll, might be replaced by other specific defaults
	if (js.contains("default_poll") && js["default_poll"].is_string())
		this->defPoll = StringToPollType(js["default_poll"]);

	if (js.contains("linux_poll") && js["linux_poll"].is_string())
		this->linuxDefPoll = StringToPollType(js["linux_poll"]);

	if (js.contains("windows_poll") && js["windows_poll"].is_string())
		this->windowsDefPoll = StringToPollType(js["windows_poll"]);
//replace default depending on system running it where applicable
#if IS_RPI
	//running from raspberry pi
	if (js.contains("linux_poll") && js["linux_poll"].is_string())
		this->defPoll = linuxDefPoll;
#elif _WIN32
	//running from windows
	if (js.contains("windows_poll") && js["windows_poll"].is_string())
		this->defPoll = windowsDefPoll;
#endif

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

	if (js.contains("processing") && js["processing"].is_string())
		this->processing = StringToProcessingType(js["processing"]);
		
}