#pragma once

#include <string>
#include <vector>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

/// <summary>
/// Representation of a Camera channel
/// </summary>
struct CamChannel
{
	/// <summary>
	/// The preferred way to display the channel in UIs
	/// with a human-readable string.
	/// </summary>
	std::string label;

	/// <summary>
	/// The cached hostname of the camera stream URI.
	/// </summary>
	std::string hostname;

	/// <summary>
	/// The format or use of the image data. For now this is
	/// an arbitrary string. In the future when more use-cases
	/// are discovered, this may become standardized.
	/// </summary>
	std::string type;

	/// <summary>
	/// The scheme/protocol of a web URI for connecting to
	/// a video stream server.
	/// </summary>
	std::string proto;

	/// <summary>
	/// The port of a video stream. If 0, try to find a default
	/// known port based off the protocol. For defaulted ports,
	/// add support as needed based off the IANA default port 
	/// registry.
	/// </summary>
	int port = 0;

	/// <summary>
	/// The endpoint of a video stream URI.
	/// </summary>
	std::string endpoint;

public:
	/// <summary>
	/// Extract CamChannels from an Equipment's client data cache.
	/// </summary>
	/// <param name="clientData">An Equipment's client data cache.</param>
	/// <returns>The found CamChannels.</returns>
	static std::vector<CamChannel> ExtractChannels(const json & clientData, const std::string& hostname);

	/// <summary>
	/// Get the default port for a supported video streaming.
	/// </summary>
	/// <param name="proto">The scheme to retrieve the port for.</param>
	/// <returns>The port for the scheme.</returns>
	static int GetDefaultPort(const std::string & proto);
};


