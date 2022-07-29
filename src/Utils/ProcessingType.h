#pragma once
#include <string>
/// <summary>
/// enum for types of processing to apply
/// </summary>
enum class ProcessingType {
	/// <summary>
	/// no processing needs to be applied
	/// </summary>
	None,

	/// <summary>
	/// apply yen thresholding
	/// </summary>
	yen_threshold,

	/// <summary>
	/// apply yen thresholding in a compressed fashion, no dialation, no floodfill
	/// </summary>
	yen_threshold_compressed,

	/// <summary>
	/// This threshold is a static number defined by the user.
	/// </summary>
	static_threshold,

	/// <summary>
	/// apply a simple threshold 2 standard deviations away from the mean
	/// </summary>
	two_stdev_from_mean,
};

/// <summary>
/// Convert a ProcessingType to a serialiable string value.
/// 
/// The name convention is made to match std::to_string() functions.
/// </summary>
/// <param name="ty">The ProcessingType to get the name of.</param>
/// <returns>
/// A serializable string version of ProcessingType that can be converted
/// back with ProcessingType().
/// </returns>
std::string to_string(ProcessingType ty);

/// <summary>
/// Convert a ProcessingType to a serialiable string value.
/// </summary>
/// <param name="ty">TThe name of a ProcessingType to convert to an enum.</param>
/// <returns>
/// An enum version of a ProcessingType string. If the string is not recognized,
/// it is defaulted to ProcessingType::None.
/// </returns>
ProcessingType StringToProcessingType(const std::string& str);