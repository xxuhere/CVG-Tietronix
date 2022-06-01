#pragma once
#include "VideoPollType.h"
#include "ProcessingType.h"
#include "nlohmann/json.hpp"
#include <string>
#include <optional>

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

	/// <summary>
	/// The width of the camera stream (if using an OpenCV implementation)
	/// 
	/// A value of 0 will use the undefined default.
	/// </summary>
	int streamWidth = 640;

	/// <summary>
	/// The height of the camera stream (if using an OpenCV implementation)
	/// 
	/// A value of 0 will use the undefined default.
	/// </summary>
	int streamHeight = 480;

	/// <summary>
	/// For algorithms that specify an explict threshold value (from 0-255)
	/// </summary>
	int thresholdExplicit = 128;

	/// <summary>
	/// The type of image processing to apply to the video frames.
	/// </summary>
	ProcessingType processing = ProcessingType::None;
};


/// <summary>
/// An extended version of cvgCamFeedLocs that contains a variable to specify
/// the ideal polling solution to use, as well as JSON serialization support.
/// </summary>
struct cvgCamFeedSource : public cvgCamFeedLocs
{
public:
	// We maintain poll types for other platforms so that if the 
	// application is runing concurrently on multiple platforms, we
	// don't destroy option information for the other platforms.

	/// <summary>
	/// The default source to us. This may not be used if the application 
	/// chooses not the respect the default or one of the other options 
	/// overrides it.
	/// </summary>
	VideoPollType defPoll = VideoPollType::OpenCVUSB_Idx;

	/// <summary>
	/// The default source for Windows, if specified.
	/// </summary>
	std::optional<VideoPollType> windowsOverRidePoll;

	/// <summary>
	/// The default source for linux, if specified.
	/// </summary>
	std::optional<VideoPollType> linuxOverRidePoll;

	/// <summary>
	/// If true, then the camera will be the target of what the
	/// threshold menu controls.
	/// </summary>
	bool menuTarg = false;

	/// <summary>
	/// Get the polling method to use for the application. Takes into 
	/// account platform overrides and the current platform.
	/// </summary>
	/// <returns></returns>
	VideoPollType GetUsedPoll() const;

	/// <summary>
	/// Get a JSON representation of the object.
	/// </summary>
	/// <returns></returns>
	json AsJSON() const;

	/// <summary>
	/// Load a JSON representation of the object.
	/// </summary>
	/// <param name="js"></param>
	void ApplyJSON(const json& js);
};