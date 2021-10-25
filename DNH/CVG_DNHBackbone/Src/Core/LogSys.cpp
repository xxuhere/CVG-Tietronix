#include "LogSys.h"
#include <iostream>

namespace CVG
{
	void LogSys::Log(const std::string& message)
	{
		// Logging implementation here
		std::cout << message << std::endl;
	}

	void LogSys::LogVerbose(const std::string& message)
	{
		if (AllowVerbose())
			Log(message);
	}
}