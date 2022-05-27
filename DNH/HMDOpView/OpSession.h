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
};