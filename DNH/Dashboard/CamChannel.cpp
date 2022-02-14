#include "CamChannel.h"

#include "ParseUtils.h"

std::vector<CamChannel> CamChannel::ExtractChannels(const json & clientData, const std::string& hostname)
{
	std::vector<CamChannel> ret;

	if(!clientData.contains("channels"))
		return ret;

	const json & jsc = clientData["channels"];
	if(!jsc.is_array())
		return ret;

	for(const json & jscEle : jsc)
	{
		if(!jscEle.is_object())
			continue;

		CamChannel addCC;
		addCC.hostname = hostname;

		if(!CVG::ParseUtils::ExtractJSONString(jscEle, "proto", addCC.proto))
			addCC.proto = "rtsp"; // Arbitrary default.

		CVG::ParseUtils::ExtractJSONString(jscEle, "type", addCC.type);

		CVG::ParseUtils::ExtractJSONString(jscEle, "endpoint", addCC.endpoint);

		CVG::ParseUtils::ExtractJSONString(jscEle, "label", addCC.label);

		bool defaultPort = true;
		std::string extractPort;
		if(CVG::ParseUtils::ExtractJSONString(jscEle, "port", extractPort))
		{
			try
			{
				int port = std::stoi(extractPort);
				addCC.port = port;
				defaultPort = false;
			}
			catch(std::exception & /*ex*/)
			{} // Do nothing
		}
		if(defaultPort)
			addCC.port = GetDefaultPort(addCC.proto);

		// Any tests to validate if the CamChannel is worth keeping. If
		// we see anything wrong with it, cull it out. We may even want
		// some way to report rejected parsed channel items.
		if(addCC.port == 0)
			continue;

		// Default a label value if nothing specified one.
		if(addCC.label.empty())
			addCC.label = std::to_string(addCC.port);

		ret.push_back(addCC);
	}

	return ret;
}

int CamChannel::GetDefaultPort(const std::string & proto)
{
	// The ports can be added on an as-needed basis, and should
	// used IANA registered ports.
	// https://www.iana.org/assignments/service-names-port-numbers/service-names-port-numbers.xhtml

	if(proto == "rtsp")
		return 554;

	return 0;
}