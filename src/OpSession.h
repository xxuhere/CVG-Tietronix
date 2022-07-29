#pragma once
#include <string>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

/// <summary>
/// Operation session data.
/// 
/// NOTE: currently UNUSED.
/// </summary>
struct OpSession
{
	// Patient's first name
	std::string patNameFirst = "__unset_firstname";

	// Patient's middle name
	std::string patNameMid = "__unset_middlename";

	// Patient's last name
	std::string patNameLast = "__unset_lastname";

	// Session name metadata
	std::string sessionName = "__unset_sessionname";

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

	json RepresentationAsJSON() const;
	
	void Apply(json& data);

	bool LoadFromFile(const std::string& filepath);

	bool SaveToFile(const std::string& filepath) const;

	void Clear();

	void Default();
};