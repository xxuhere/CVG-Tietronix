#pragma once
#include <string>
/// <summary>
/// Specify where the polled image should come from.
/// </summary>
enum class VideoPollType
{
	/// <summary>
	/// Do not poll - camera thread will idle until the poll type is reset.
	/// </summary>
	Deactivated,

	/// <summary>
	/// Poll from OpenCV for USB webcams.
	/// </summary>
	OpenCVUSB_Idx,

	/// <summary>
	/// Poll from OpenCV for USB webcams.
	/// </summary>
	OpenCVUSB_Named,

	/// <summary>
	/// Poll from OpenCV through a RTSP internet stream.
	/// </summary>
	Web,

	/// <summary>
	/// Poll from an external application.
	/// </summary>
	External,

	/// <summary>
	/// Simulate polling by showing a text image.
	/// </summary>
	Image
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
std::string to_string(VideoPollType ty);

/// <summary>
/// Convert a VideoPollType to a serialiable string value.
/// </summary>
/// <param name="ty">TThe name of a VideoPollType to convert to an enum.</param>
/// <returns>
/// An enum version of a VideoPollType string. If the string is not recognized,
/// it is defaulted to VideoPollType::Deactivated.
/// </returns>
VideoPollType StringToPollType(const std::string& str);