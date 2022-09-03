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

/// <summary>
/// Represents the loaded data for an entire Carousel. Besides holding
/// the serialized data, it also has the methods for loading.
/// </summary>
class CarouselSystemData
{
public:
	std::vector<CarouselData> entries;

public:
	void Clear();

	/// <summary>
	/// Load a JSON's content into carouselEntries.
	/// 
	/// This handles two types of data, an array of entries, or a string
	/// reference to another file that has an array. This allows defining
	/// carousel entries externally in a form that's reusable.
	/// </summary>
	/// <param name="js">
	/// The json object to load. This should be either an array or
	/// (filename) string.</param>
	/// <returns></returns>
	bool ApplyJSON(const json& js);

	/// <summary>
	/// Copy the entries of another CarouselSystemData.
	/// </summary>
	/// <param name="csdSrc">The data to copy</param>
	void AddCopyCatEntries(CarouselSystemData& csdSrc);

	/// <summary>
	/// Get a JSON representation of the object.
	/// </summary>
	json AsJSON() const;
};