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
	/// not implemented
	/// </summary>
	
};

/// <summary>
/// Convert a VideoPollType to a serialiable string value.
/// 
/// The name convention is made to match std::to_string() functions.
/// </summary>
/// <param name="ty">The VideoPollType to get the name of.</param>
/// <returns>
/// A serializable string version of VideoPollType that can be converted
/// back with StringToPollType().
/// </returns>
std::string to_string(ProcessingType ty);

/// <summary>
/// Convert a VideoPollType to a serialiable string value.
/// </summary>
/// <param name="ty">TThe name of a VideoPollType to convert to an enum.</param>
/// <returns>
/// An enum version of a VideoPollType string. If the string is not recognized,
/// it is defaulted to VideoPollType::Deactivated.
/// </returns>
ProcessingType StringToProcessingType(const std::string& str);