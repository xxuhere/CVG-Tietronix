#pragma once

#include <string>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace CVG {namespace ParseUtils
{
	/// <summary>
	/// Given a JSON object that contains an expected string
	/// member, verify the member exists and extract it.
	/// </summary>
	/// <param name="js">The JSON object that contains the string member.</param>
	/// <param name="name">The name of the string member.</param>
	/// <param name="strOut">
	/// The return string value of the member.
	/// 
	/// Only valid if the function returns true.
	/// </param>
	/// <returns>
	/// True if the string value was found as a string. Else, false.
	/// </returns>
	bool ExtractJSONString(
		const json& js,
		const std::string& name,
		std::string& strOut);
}}