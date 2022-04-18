#pragma once
#include "VideoPollType.h"
#include "nlohmann/json.hpp"
#include <string>

using json = nlohmann::json;

/// <summary>
/// A list of options of where a cam feed can come from.
/// </summary>
class cvgCamFeedLocs
{
public:
	// For serialization (in cvgCamFeedSource),
	// these member defaults are not expected to work 
	// "out of the box", but instead give something that
	// looks similar to what expected of the final version
	// that a user should have filled out.

	// <summary>
	/// The index for OpenCV
	/// </summary>
	int camIndex = 0;

	/// <summary>
	/// The URI for web server locations
	/// </summary>
	std::string uriSource = "rtsp://localhost:554/unicast";

	/// <summary>
	/// The path for linux device paths
	/// </summary>
	std::string devicePath = "/dev/video";

	/// <summary>
	/// If the poll type is External, we're expecting to receive
	/// raw bytes from a spawned pipe.
	/// </summary>
	std::string externalPipeCmd = "raspiyuv";

	/// <summary>
	/// If the poll type is a static image, the path to pull the
	/// image from.
	/// </summary>
	std::string staticImagePath = "testimage.png";

	/// <summary>
	/// The number of (1 byte channels) that externalPipeCmd
	/// should pipe in. There's no handshaking involved with the program
	/// so this needs to be explicitly known and defined by us.
	/// </summary>
	int channelCtFromPipe = 3;

	/// <summary>
	/// The width of the image if we're streaming it from externalPipeCmd;
	/// </summary>
	int pipeWidth = 640;

	/// <summary>
	/// The height of the image if we're streaming it from externalPipeCmd;
	/// </summary>
	int pipeHeight = 480;
};


/// <summary>
/// An extended version of cvgCamFeedLocs that contains a variable to specify
/// the ideal polling solution to use, as well as JSON serialization support.
/// </summary>
struct cvgCamFeedSource : public cvgCamFeedLocs
{
public:
	// The default source to us. This may not be used if the 
	// application chooses not the repect the default.
	VideoPollType defPoll = VideoPollType::OpenCVUSB_Idx;

	json AsJSON() const;
	void ApplyJSON(const json& js);
};