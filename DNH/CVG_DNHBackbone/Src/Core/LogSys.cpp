#include "LogSys.h"
#include <iostream>
#include <mutex>
namespace CVG
{
	void LogSys::Log(const std::string& message)
	{
		// It's been observed that the app will freeze sometimes
		// when trying to push content to std::cout. The current
		// theory is that 
		// << std::endl 
		// will flush the buffer which is somehow the culprit when
		// mixed with the issue of threading.
		static std::mutex coutMutex;
		std::lock_guard<std::mutex> guard(coutMutex);

		// Logging implementation here
		std::cout << (message + "\n");// << std::endl;
	}

	void LogSys::LogVerbose(const std::string& message)
	{
		if (AllowVerbose())
			Log(message);
	}
}