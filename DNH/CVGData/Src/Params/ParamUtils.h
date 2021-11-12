#pragma once

#include <string>

#include "Param.h"
#include "ParamBool.h"
#include "ParamEnum.h"
#include "ParamFloat.h"
#include "ParamInt.h"
#include "ParamString.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace CVG
{
	/// <summary>
	/// Utilities involving the Param class.
	/// </summary>
	class ParamUtils
	{
	public:
		/// <summary>
		/// Parse a JSON with a Param definition and create a 
		/// matching description.
		/// 
		/// If the JSON is expected to be a Param definition
		/// but the type is known, see ParamUtils::Parse().
		/// </summary>
		/// <param name="js">The JSON with the Param description.</param>
		/// <param name="error">
		/// If the return is nullptr, a parsing error has occured. The
		/// error output parameter will contain a readable reason why.
		/// 
		/// If the function is successful, error should be ignored.
		/// </param>
		/// <returns>The created Param if successful. Else, nullptr.</returns>
		static ParamSPtr Parse(const json& js, std::string& error);

		/// <summary>
		/// Given a Param JSON definition known to be
		/// of type int, create a Param of it.
		/// 
		/// This function assumes some known elements have already
		/// been parsed, such as the id, label, etc. The function
		/// will search for other non-specified definition data.
		/// 
		/// If the JSON is expected to be a Param definition
		/// but the type is known, see ParamUtils::Parse().
		/// </summary>
		/// <param name="js">The JSON int Param definition to parse.</param>
		/// <param name="id">The known Param id.</param>
		/// <param name="label">The known Param label.</param>
		/// <param name="category">The known Param category.</param>
		/// <param name="error">
		/// The reason for the error if the return value is nullptr.
		/// </param>
		/// <returns>The created Param.</returns>
		static ParamSPtr ParseInt(
			const json& js, 
			const std::string& id, 
			const std::string& label, 
			const std::string& category, 
			const std::string& unit,
			std::string& error);

		/// <summary>
		/// Given a Param JSON definition known to be
		/// of type string, create a Param of it.
		///
		/// This function assumes some known elements have already
		/// been parsed, such as the id, label, etc. The function
		/// will search for other non-specified definition data.
		/// 
		/// If the JSON is expected to be a Param definition
		/// but the type is known, see ParamUtils::Parse().
		/// </summary>
		/// <param name="js">The JSON string Param definition to parse.</param>
		/// <param name="id">The known Param id.</param>
		/// <param name="label">The known Param label.</param>
		/// <param name="category">The known Param category.</param>
		/// <param name="error">
		/// The reason for the error if the return value is nullptr.
		/// </param>
		/// <returns>The created Param.</returns>
		static ParamSPtr ParseString(
			const json& js,
			const std::string& id,
			const std::string& label,
			const std::string& category,
			const std::string& unit,
			std::string& error);

		/// <summary>
		/// Given a Param JSON definition known to be
		/// of type bool, create a Param of it.
		/// 
		/// This function assumes some known elements have already
		/// been parsed, such as the id, label, etc. The function
		/// will search for other non-specified definition data.
		/// 
		/// If the JSON is expected to be a Param definition
		/// but the type is known, see ParamUtils::Parse().
		/// </summary>
		/// <param name="js">The JSON bool Param definition to parse.</param>
		/// <param name="id">The known Param id.</param>
		/// <param name="label">The known Param label.</param>
		/// <param name="category">The known Param category.</param>
		/// <param name="error">
		/// The reason for the error if the return value is nullptr.
		/// </param>
		/// <returns>The created Param.</returns>
		static ParamSPtr ParseBool(
			const json& js,
			const std::string& id,
			const std::string& label,
			const std::string& category,
			const std::string& unit,
			std::string& error);

		/// <summary>
		/// Given a Param JSON definition known to be
		/// of type enum, create a Param of it.
		/// 
		/// This function assumes some known elements have already
		/// been parsed, such as the id, label, etc. The function
		/// will search for other non-specified definition data.
		/// 
		/// If the JSON is expected to be a Param definition
		/// but the type is known, see ParamUtils::Parse().
		/// </summary>
		/// <param name="js">The JSON enum Param definition to parse.</param>
		/// <param name="id">The known Param id.</param>
		/// <param name="label">The known Param label.</param>
		/// <param name="category">The known Param category.</param>
		/// <param name="error">
		/// The reason for the error if the return value is nullptr.
		/// </param>
		/// <returns>The created Param.</returns>
		static ParamSPtr ParseEnum(
			const json& js,
			const std::string& id,
			const std::string& label,
			const std::string& category,
			const std::string& unit,
			std::string& error);

		/// <summary>
		/// Given a Param JSON definition known to be
		/// of type float, create a Param of it.
		/// 
		/// This function assumes some known elements have already
		/// been parsed, such as the id, label, etc. The function
		/// will search for other non-specified definition data.
		/// 
		/// If the JSON is expected to be a Param definition
		/// but the type is known, see ParamUtils::Parse().
		/// </summary>
		/// <param name="js">The JSON float Param definition to parse.</param>
		/// <param name="id">The known Param id.</param>
		/// <param name="label">The known Param label.</param>
		/// <param name="category">The known Param category.</param>
		/// <param name="error">
		/// The reason for the error if the return value is nullptr.
		/// </param>
		/// <returns>The created Param.</returns>
		static ParamSPtr ParseFloat(
			const json& js,
			const std::string& id,
			const std::string& label,
			const std::string& category,
			const std::string& unit,
			std::string& error);
	};
}