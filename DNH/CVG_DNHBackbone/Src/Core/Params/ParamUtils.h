#pragma once

#include <string>

#include "Param.h"
#include "ParamBool.h"
#include "ParamEnum.h"
#include "ParamFloat.h"
#include "ParamInt.h"
#include "ParamString.h"
#include "json.hpp"

using json = nlohmann::json;

namespace CVG
{
	class ParamUtils
	{
	public:
		/// <summary>
		/// Parse a JSON with a Param description and create a 
		/// matching description.
		/// </summary>
		/// <param name="js">The JSON with the Param description.</param>
		/// <param name="error">
		/// If 
		/// </param>
		/// <returns>The created Param if successful. Else, nullptr.</returns>
		static ParamSPtr Parse(const json& js, std::string& error);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="js"></param>
		/// <param name="id"></param>
		/// <param name="label"></param>
		/// <param name="category"></param>
		/// <param name="error"></param>
		/// <returns></returns>
		static ParamSPtr ParseInt(
			const json& js, 
			const std::string& id, 
			const std::string& label, 
			const std::string& category, 
			const std::string& unit,
			std::string& error);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="js"></param>
		/// <param name="id"></param>
		/// <param name="label"></param>
		/// <param name="category"></param>
		/// <param name="error"></param>
		/// <returns></returns>
		static ParamSPtr ParseString(
			const json& js,
			const std::string& id,
			const std::string& label,
			const std::string& category,
			const std::string& unit,
			std::string& error);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="js"></param>
		/// <param name="id"></param>
		/// <param name="label"></param>
		/// <param name="category"></param>
		/// <param name="error"></param>
		/// <returns></returns>
		static ParamSPtr ParseBool(
			const json& js,
			const std::string& id,
			const std::string& label,
			const std::string& category,
			const std::string& unit,
			std::string& error);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="js"></param>
		/// <param name="id"></param>
		/// <param name="label"></param>
		/// <param name="category"></param>
		/// <param name="error"></param>
		/// <returns></returns>
		static ParamSPtr ParseEnum(
			const json& js,
			const std::string& id,
			const std::string& label,
			const std::string& category,
			const std::string& unit,
			std::string& error);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="js"></param>
		/// <param name="id"></param>
		/// <param name="label"></param>
		/// <param name="category"></param>
		/// <param name="error"></param>
		/// <returns></returns>
		static ParamSPtr ParseFloat(
			const json& js,
			const std::string& id,
			const std::string& label,
			const std::string& category,
			const std::string& unit,
			std::string& error);
	};
}