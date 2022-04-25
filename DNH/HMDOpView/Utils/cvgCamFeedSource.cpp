#include "cvgCamFeedSource.h"

json cvgCamFeedSource::AsJSON() const
{
	json ret = json::object();

	ret["default_poll"	] = to_string(this->defPoll);
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
	if(js.contains("default_poll") && js["default_poll"].is_string())
		this->defPoll = StringToPollType(js["default_poll"]);

	if(js.contains("index") && js["index"].is_number())
		this->camIndex = js["index"];

	if(js.contains("uri") && js["uri"].is_string())
		this->uriSource = js["uri"];

	if(js.contains("devpath") && js["dev_path"].is_string())
		this->uriSource = js["devpath"];

	if(js.contains("pipecmd") && js["pipe_cmd"].is_string())
		this->externalPipeCmd = js["pipecmd"];

	if(js.contains("pipechans") && js["pipe_chans"].is_string())
		this->channelCtFromPipe = js["pipechans"];

	if(js.contains("pipe_width") && js["pipe_width"].is_number())
		this->pipeWidth = js["pipe_width"];

	if(js.contains("pipe_height") && js["pipe_height"].is_number())
		this->pipeHeight = js["pipe_height"];

	if(js.contains("static_img") && js["static_img"].is_string())
		this->staticImagePath = js["static_img"];

	if (js.contains("processing") && js["processing"].is_string())
		this->processing = StringToProcessingType(js["processing"]);
		
}