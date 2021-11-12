#pragma once
#include <string>

namespace CVG
{
	/// <summary>
	/// The category of error elevations for DNH API requests.
	/// </summary>
	enum class ErrorTy
	{
		Log,
		Warning,
		Error,

		// Fatal implies a e-stop shutdown and forcing
		// fail-safes.
		Fatal,

		// Used as a counter, should not be used in practice.
		Totalnum 
	};

	/// <summary>
	/// Convert an API string value of an error to its C++ enumeration.
	/// </summary>
	/// <param name="str">The value to convert.</param>
	/// <returns>The converted C++ enum.</returns>
	ErrorTy ConvertToErrorTy(const std::string& str);

	/// <summary>
	/// Convert an error enumeration to its API string value.
	/// </summary>
	/// <param name="dt">The value to convert.</param>
	/// <returns>The converted string value used in the API.</returns>
	std::string ConvertToString(ErrorTy dt);
}