#include "cvgCamFeedSource.h"
#include <iostream>
#include "multiplatform.h"

// These strings names follow more of a JavaScript convention,
// because,... JSON (The JS in JSON)
static const char* szKey_DefPoll		= "default_poll";
static const char* szKey_WinPoll		= "windows_poll";
static const char* szKey_LinuxPoll		= "linux_poll";
static const char* szKey_Index			= "index";
static const char* szKey_MMALIndex		= "mmal_index";
static const char* szKey_Uri			= "uri";
static const char* szKey_DevPath		= "dev_path";
static const char* szKey_PipeCmd		= "pipe_cmd";
static const char* szKey_PipeChans		= "pipe_chans";
static const char* szKey_PipeWidth		= "pipe_width";
static const char* szKey_PipeHeight		= "pipe_height";
static const char* szKey_StreamWidth	= "stream_width";
static const char* szKey_StreamHeight	= "stream_height";
static const char* szKey_StaticImg		= "static_img";
static const char* szKey_MenuTarg		= "menu_targ";
static const char* szKey_Processing		= "processing";
static const char* szKey_FlipHoriz		= "flip_horiz";
static const char* szKey_FlipVert		= "flip_vert";
static const char* szKey_VideoExpMicro	= "video_exposure_micro";

json cvgCamFeedSource::AsJSON() const
{
	// WHEN ADDING/MODIFYING ITEMS HERE, 
	// REMEMBER TO MATCH THE LOGIC WITH 
	// AN EQUIVALENT IN ApplyJSON().

	json ret = json::object();

	ret[szKey_DefPoll	] = to_string(this->defPoll);

	if(this->windowsOverRidePoll.has_value())
		ret[szKey_WinPoll	] = to_string(this->windowsOverRidePoll.value());
	if(this->linuxOverRidePoll.has_value())
		ret[szKey_LinuxPoll	] = to_string(this->linuxOverRidePoll.value());

	ret[szKey_Index			] = this->camIndex;
	ret[szKey_MMALIndex		] = this->camMMALIdx;
	ret[szKey_Uri			] = this->uriSource;
	ret[szKey_DevPath		] = this->devicePath;
	ret[szKey_PipeCmd		] = this->externalPipeCmd;
	ret[szKey_PipeChans		] = this->channelCtFromPipe;
	ret[szKey_PipeWidth		] = this->pipeWidth;
	ret[szKey_PipeHeight	] = this->pipeHeight;
	ret[szKey_StreamWidth	] = this->streamWidth;
	ret[szKey_StreamHeight	] = this->streamHeight;
	ret[szKey_StaticImg		] = this->staticImagePath;
	ret[szKey_MenuTarg		] = this->menuTarg;
	ret[szKey_FlipHoriz		] = this->flipHorizontal;
	ret[szKey_FlipVert		] = this->flipVertical;
	ret[szKey_VideoExpMicro	] = this->videoExposureTime;

	if(this->processing == ProcessingType::static_threshold)
		ret[szKey_Processing] = this->thresholdExplicit;
	else
		ret[szKey_Processing] = to_string(this->processing);

	return ret;
}

void cvgCamFeedSource::ApplyJSON(const json& js)
{
	// WHEN ADDING/MODIFYING ITEMS HERE, REMEMBER TO 
	// MATCH THE LOGIC WITH AN EQUIVALENT IN AsJSON().

	//Default poll, might be replaced by other specific defaults
	if (js.contains(szKey_DefPoll) && js[szKey_DefPoll].is_string())
		this->defPoll = StringToPollType(js[szKey_DefPoll]);

	if (js.contains(szKey_LinuxPoll) && js[szKey_LinuxPoll].is_string())
		this->linuxOverRidePoll = StringToPollType(js[szKey_LinuxPoll]);

	if (js.contains(szKey_WinPoll) && js[szKey_WinPoll].is_string())
		this->windowsOverRidePoll = StringToPollType(js[szKey_WinPoll]);

	if(js.contains(szKey_Index) && js[szKey_Index].is_number())
		this->camIndex = js[szKey_Index];

	if(js.contains(szKey_MMALIndex) && js[szKey_MMALIndex].is_number())
		this->camMMALIdx = js[szKey_MMALIndex];

	if(js.contains(szKey_Uri) && js[szKey_Uri].is_string())
		this->uriSource = js[szKey_Uri];

	if(js.contains(szKey_DevPath) && js[szKey_DevPath].is_string())
		this->devicePath = js[szKey_DevPath];

	if(js.contains(szKey_PipeCmd) && js[szKey_PipeCmd].is_string())
		this->externalPipeCmd = js[szKey_PipeCmd];

	if(js.contains(szKey_PipeChans) && js[szKey_PipeChans].is_string())
		this->channelCtFromPipe = js[szKey_PipeChans];

	if(js.contains(szKey_PipeWidth) && js[szKey_PipeWidth].is_number())
		this->pipeWidth = js[szKey_PipeWidth];

	if(js.contains(szKey_PipeHeight) && js[szKey_PipeHeight].is_number())
		this->pipeHeight = js[szKey_PipeHeight];

	if(js.contains(szKey_StreamWidth) && js[szKey_StreamWidth].is_number_integer())
		this->streamWidth = js[szKey_StreamWidth];

	if(js.contains(szKey_StreamHeight) && js[szKey_StreamHeight].is_number_integer())
		this->streamHeight = js[szKey_StreamHeight];

	if(js.contains(szKey_StaticImg) && js[szKey_StaticImg].is_string())
		this->staticImagePath = js[szKey_StaticImg];

	if(js.contains(szKey_MenuTarg) && js[szKey_MenuTarg].is_boolean())
		this->menuTarg = js[szKey_MenuTarg];

	if(js.contains(szKey_FlipHoriz) && js[szKey_FlipHoriz].is_boolean())
		this->flipHorizontal = js[szKey_FlipHoriz];

	if(js.contains(szKey_FlipVert) && js[szKey_FlipVert].is_boolean())
		this->flipVertical = js[szKey_FlipVert];

	if(js.contains(szKey_VideoExpMicro) && js[szKey_VideoExpMicro].is_number())
		this->videoExposureTime = js[szKey_VideoExpMicro];

	if (js.contains(szKey_Processing))
	{
		if(js[szKey_Processing].is_string())
			this->processing = StringToProcessingType(js[szKey_Processing]);
		else if(js[szKey_Processing].is_number()) // Arguable should be is_number_integer()
		{
			this->processing = ProcessingType::static_threshold;
			this->thresholdExplicit = js[szKey_Processing];
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