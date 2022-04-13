#include "VideoPollType.h"

std::string to_string(VideoPollType ty)
{
	switch(ty)
	{
	case VideoPollType::OpenCVUSB_Idx:
		return "cvusb";

	case VideoPollType::OpenCVUSB_Named:
		return "devpath";

	case VideoPollType::Web:
		return "url";

	case VideoPollType::External:
		return "external";

	case VideoPollType::Image:
		return "static";
	}

	return "deactivated";
}

VideoPollType StringToPollType(const std::string& str)
{
	if(str == "cvusb")
		return VideoPollType::OpenCVUSB_Idx;
	if(str == "devpath")
		return VideoPollType::OpenCVUSB_Named;

	if(str == "url")
		return VideoPollType::Web;

	if(str == "external")
		return VideoPollType::External;

	if(str == "static")
		return VideoPollType::Image;

	return VideoPollType::Deactivated;
}