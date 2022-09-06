#pragma once

#include "nlohmann/json.hpp"
#include "cvgCamFeedSource.h"
#include "CarouselData.h"
#include <string>
#include <vector>

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
	int viewportX = 640;

	/// <summary>
	/// The height of the video viewport.
	/// </summary>
	int viewportY = 480;

	// The composites should probably maintain the same aspect
	// ratio (if not literally the same values) as the viewportX
	// and viewportY, since they contain pretty much the same
	// content.

	/// <summary>
	/// The width of the composited video.
	/// </summary>
	int compositeWidth = 640;

	/// <summary>
	/// The height of the composited video.
	/// </summary>
	int compositeHeight = 480;

	/// <summary>
	/// X pixel offset of the viewport from the center.
	/// </summary>
	int viewportOffsX = 0;

	/// <summary>
	/// Y pixel offset of the viewport from the center.
	/// </summary>
	int viewportOffsY = 0;

	/// <summary>
	/// If true, the application should be fullscreen. Else, it will
	/// be windowed. The resolution of the window is not currently 
	/// definable - although the window should be resizeable.
	/// </summary>
	bool fullscreen = true;

	/// <summary>
	/// The X pixel offset of the mousepad from the center.
	/// </summary>
	int mousepadX = 0;

	/// <summary>
	/// The Y pixel offset of the mousepad from the center.
	/// </summary>
	int mousepadY = 500;

	/// <summary>
	/// The scale of the mousepad.
	/// </summary>
	float mousepadScale = 0.4f;

	/// <summary>
	/// If true, UISys should have debug drag enabled.
	/// </summary>
	bool drawUIDebug = false;

	/// <summary>
	/// Camera options. While it's represented as a generic vector,
	/// this is expected to always be of size 2.
	/// </summary>
	std::vector<cvgCamFeedSource> feedOpts;

	CarouselSystemData caroBody;
	CarouselSystemData caroSysSeries;
	CarouselSystemData caroSysOrient;

public:
	cvgOptions(int defSources, bool sampleCarousels = true);

	/// <summary>
	/// Load an options JSON file into the object.
	/// </summary>
	/// <param name="filepath">The filepath of the JSON file to load.</param>
	/// <returns>
	/// True if successful; else, the specified filepath could not be
	/// processed.
	/// </returns>
	bool LoadFromFile(const std::string& filepath);

	/// <summary>
	/// Save the object as a JSON file.
	/// </summary>
	/// <param name="filepath">The filepath to save to.</param>
	/// <returns>True if successfully saved.</returns>
	bool SaveToFile(const std::string& filepath) const;

	// Apply can work with limited data -
	// i.e., if not everything  from a standard options file is 
	// available, only the available data will be set in the invoking object.
	void Apply(json& data);

	/// <summary>
	/// Check what camera index is designated the menu target. This
	/// will be the first entry found to have menuTarg set to true.
	/// </summary>
	/// <returns>
	/// The index found to be the menu target, or -1 if none was found.
	/// </returns>
	int FindMenuTargetIndex() const;

	/// <summary>
	/// Get the object's data as a JSON object.
	/// </summary>
	json RepresentAsJSON() const;

	/// <summary>
	/// Reset object to defaults.
	/// </summary>
	void Clear();
};