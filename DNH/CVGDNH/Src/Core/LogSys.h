#pragma once
#include <string>

namespace CVG
{
	/// <summary>
	/// The logging system for the DNH.
	/// 
	/// All logged messages in the DNH should go through this 
	/// system instead of directly writing to a file or the console.
	/// 
	/// This is to ensure they all use the same logging implementation
	/// that can be controlled and potentially changed effectively.
	/// </summary>
	class LogSys
	{
	public:

		static inline bool AllowVerbose() 
		{ return true; } // For now, everything is verbose

		/// <summary>
		/// Log a message.
		/// </summary>
		/// <param name="message">The message to log.</param>
		static void Log(const std::string& message);

		/// <summary>
		/// Log a message that isn't worth showing unless
		/// verbose messages are allowed.
		/// </summary>
		/// <param name="message">The verbose message to log.</param>
		static void LogVerbose(const std::string& message);
	};
}