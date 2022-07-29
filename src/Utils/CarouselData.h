#pragma once

#include "nlohmann/json.hpp"
using json = nlohmann::json;

/// <summary>
/// The basic data for the carousel. This class is extended
/// in Carousel/Carousel.h, by class Carousel::Entry.
/// </summary>
class CarouselData
{
public:
	/// <summary>
	/// The id of the data. This can be used to address the
	/// carousel entry programatically without needing to know
	/// its runtime array index.
	/// 
	/// This should be unique for all CarouselDatas loaded at
	/// any given moment.
	/// </summary>
	std::string id;

	/// <summary>
	/// The icon to show for the entry.
	/// </summary>
	std::string iconFilepath;

	/// <summary>
	/// The label of the entry shown in the UI. If possible, the
	/// convention is to make this at most 4 characters, all caps.
	/// </summary>
	std::string label;

	/// <summary>
	/// The caption description of the entry.
	/// </summary>
	std::string caption;

public:
	CarouselData();
	CarouselData(
		const std::string& id, 
		const std::string& iconFilepath,
		const std::string& label,
		const std::string& caption);

	/// <summary>
	/// Get a JSON representation of the object.
	/// </summary>
	json AsJSON() const;

	/// <summary>
	/// Load a JSON representation of the object.
	/// </summary>
	/// <param name="js"></param>
	/// <returns>
	/// The success of the load attempt. If the json is missing expected
	/// elements, false will be returned. If there are missing items, a 
	/// partial load will occur but the data should not be considered 
	/// valid and usable.
	/// </returns>
	bool ApplyJSON(const json& js);
};