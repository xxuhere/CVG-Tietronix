#pragma once

#include "nlohmann/json.hpp"
#include <string>

using json = nlohmann::json;

/// <summary>
/// Funtion to manage options in the HMD app that can be
/// stored and saved externally as a text file format.
/// 
/// Options are save for two reasons:
/// - As a way to configure the application for users and store preferences.
/// - As a way to quickly change variables during developer iteration.
/// 
/// Some variables handled may exist in a grey area that are
/// both for user preferences and development tooling.
/// 
/// The options that this class deals with will exist in 
/// 3 forms:
/// - As a JSON representation, both as files, and in nlohmann::json objects.
/// - Stored in runtime variables directly in the cvgOptions. This is both
/// to make sure it's in a easy and ready to use form for users of the 
/// cvgOptions class, as well as ensuring format changes create compile-time
/// errors to alert developers before deployment.
/// - In the application, which will pull and integrate variables from
/// the cvgOptions instead of referencing from the cvgOptions. This should be
/// done so that the cvgOptions is only used as a utility for pulling and 
/// pushing serialized options, and not as a runtime store for the actual
/// system values.
/// </summary>
class cvgOptions
{
public:
	std::string testImagePath = "";

	/// <summary>
	/// The width of the video viewport.
	/// </summary>
	int viewportX;

	/// <summary>
	/// The height of the video viewport.
	/// </summary>
	int viewportY;
public:
	cvgOptions();

	/// <summary>
	/// 
	/// </summary>
	/// <param name="filepath"></param>
	/// <returns></returns>
	bool LoadFromFile(const std::string& filepath);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="filepath"></param>
	/// <returns></returns>
	bool SaveToFile(const std::string& filepath) const;

	// Apply can work with limited data -
	// i.e., if not everything  from a standard options file is 
	// available, only the available data will be set in the invoking object.
	void Apply(json& data);

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	json RepresentAsJSON() const;

	/// <summary>
	/// 
	/// </summary>
	void Clear();
};