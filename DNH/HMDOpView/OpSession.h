#pragma once
#include <string>

/// <summary>
/// Operation session data.
/// 
/// NOTE: currently UNUSED.
/// </summary>
struct OpSession
{
	// Patient's first name
	std::string patNameFirst;

	// Patient's middle name
	std::string patNameMid;

	// Patient's last name
	std::string patNameLast;

	// Session name metadata
	std::string sessionName;

	void SetName(
		const std::string& first, 
		const std::string& mid, 
		const std::string& last);

	void SetSession(const std::string& session);

	/// <summary>
	/// Generate a unique string that can represent the session
	/// as a name valid on file/folder names.
	/// </summary>
	/// <returns>
	/// The valid string, or _invalid_ if there are no valid options.
	/// </returns>
	std::string GenerateSessionPrefix() const;
};