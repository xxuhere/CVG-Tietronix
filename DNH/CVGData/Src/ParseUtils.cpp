#include "ParseUtils.h"

namespace CVG { namespace ParseUtils
{
	bool ExtractJSONString(
		const json& js,
		const std::string& name,
		std::string& strOut)
	{
		if (!js.contains(name))
			return false;

		if (!js[name].is_string())
			return false;

		strOut = js[name];
		return true;
	}
}}