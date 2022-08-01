#pragma once
#include <string>

/// <summary>
/// Miscellaneous tools for Dicom file management.
/// 
/// We call it "misc" utils because the term DicomUtils
/// is already taken for the folder name.
/// </summary>
namespace DicomMiscUtils
{
	/// <summary>
	/// Get the current date as a dicom date string.
	/// </summary>
	std::string GetCurrentDateString();

	/// <summary>
	/// Get the current machine time as a dicom time string.
	/// </summary>
	std::string GetCurrentTime();
}