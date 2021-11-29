#pragma once
#include <string>
#include <memory>

#include "../Types/DataType.h"

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace CVG 
{
	class Param;
	typedef std::shared_ptr<Param> ParamSPtr;

	/// <summary>
	/// Return value for a Param operation that has 3 
	/// possible ending states.
	/// </summary>
	enum class SetRet
	{
		/// <summary>
		/// The operation was successful. Nothing more
		/// needs to be done.
		/// </summary>
		Success,

		/// <summary>
		/// The operation was successful. An event should
		/// be sent.
		/// </summary>
		Submit,

		/// <summary>
		/// The operation was not successful. Most likely because
		/// a type conversion was needed that is undefined.
		/// </summary>
		Invalid
	};

	/// <summary>
	/// The base class for CVG Equipment parameters. These are
	/// introspective values for things connected through the
	/// DNH realtime bus.
	/// </summary>
	class Param 
	{
	public:
		/// <summary>
		/// Each Param will contain multiple values. The actual
		/// value (Current), as well as other supporting values
		/// to provide extra support features.
		/// </summary>
		enum class ValTy
		{
			// The current value of the Param.
			Current,

			// The value the parameter will be set to on a reset event.
			// This is optional. If a default doesn't exist for a Param,
			// it will remain untouched during the reset event.
			Default,

			// The minimum value. Not all Param data types support having
			// a minimum value. This value define the (inclusive) lower
			// limit the Current can be.
			Min,

			// The maximum value. Not all Param data types support having
			// a maximum value. This value defines the (inclusive) upper
			// limit the Current can be.
			Max,

			// The fail-safe prameter. If the parameter has this specified,
			// it will be forced to this value when an error or unexpected
			// disconnection happens with the equipment.
			Fail
		};

	private:
		// The type of data the Param supports. This will also
		// specify what kind of subclass the Param is (see 
		// comments for DataType for more information).
		/// <see cref="DataType"/>
		DataType type;

		/// <summary>
		/// The human-readable name of the Param. If unspecified,
		/// it will be defaulted to the API id.
		/// </summary>
		std::string label;

		/// <summary>
		/// The value used to identify the parameter in the API.
		/// </summary>
		std::string id;

		/// <summary>
		/// Optional. The category of the Param. This is metadata used
		/// to give hints on how procedrually generated GUIs should
		/// group this Param in respect to other Params. If GUI systems 
		/// Params respect the category, Params that share the same category
		/// will be visually grouped together.
		/// </summary>
		std::string category;

		/// <summary>
		/// Optional. The unit of the value. This can be anything but certain
		/// units may have built-in support in the DNH system and GUI system.
		/// </summary>
		std::string unit;

	protected:

		/// <summary>
		/// Utility function for GetJSON() to add subclass implementation-specific
		/// content to the return value.
		/// </summary>
		/// <param name="js">The json to add implementation-specific data to.</param>
		/// <see cref="Param::GetJSON()"/>
		virtual void ModifyJSONImpl(json& js) = 0;

	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="id">The Param's id.</param>
		/// <param name="label">The Param's human-readable label.</param>
		/// <param name="category">The Param's category for GUI organization.</param>
		/// <param name="unit">The Param's unit type.</param>
		/// <param name="type">The Param's data type.</param>
		Param(
			const std::string & id,
			const std::string & label,
			const std::string & category,
			const std::string & unit,
			DataType type);

		/// <summary>
		/// Public accessor to the data type.
		/// </summary>
		/// <returns>The Param's data type.</returns>
		DataType Type() const 
		{
			return this->type;
		}

		/// <summary>
		/// Public accessor to the readable label.
		/// </summary>
		/// <returns>The Param's label.</returns>
		std::string GetLabel() const 
		{
			return this->label;
		}

		/// <summary>
		/// Public accessor to the API id.
		/// </summary>
		/// <returns>The Param's API id.</returns>
		std::string GetID() const 
		{
			return this->id;
		}

		/// <summary>
		/// Check if the Param implements specific type, where
		/// the type is specified by the name of the JSON api
		/// type.
		/// </summary>
		/// <param name="ty">The typename, as specified in the
		/// DNH API.
		/// </param>
		/// <returns>True if matches, else false.</returns>
		bool IsJSONType(const std::string& ty) const;

		/// <summary>
		/// Get the data type the Param implements, in the form
		/// of how it's referenced in the JSON API.
		/// </summary>
		/// <returns>A string representing the typename in the DNH API.</returns>
		std::string GetJSONType() const;

		/// <summary>
		/// The value of the Param as a string.
		/// </summary>
		/// <returns>The value of the Param.</returns>
		virtual std::string StrValue() = 0;

		/// <summary>
		/// Query if the Param has a minimum limit.
		/// </summary>
		/// <returns>If true, the Param has a minimum limit. Else, false.</returns>
		virtual bool HasMin() = 0;

		/// <summary>
		/// Query if the Param has a maximum limit.
		/// </summary>
		/// <returns>If true, the Param has a maximum limit. Else, false.</returns>
		virtual bool HasMax() = 0;

		/// <summary>
		/// Query if the Param has a fail-safe override.
		/// </summary>
		/// <returns>If true, the Param has a fail value. Else, false.</returns>
		virtual bool HasFail() = 0;

		/// <summary>
		/// Set a Param value to an int.
		/// </summary>
		/// <param name="value">The value.</param>
		/// <param name="ty">The part of the Param to set.</param>
		/// <returns>If true, the value was successfully set. Else, false</returns>
		virtual SetRet SetValue(int value, ValTy ty = ValTy::Current) = 0;

		/// <summary>
		/// Get a Param value as an int.
		/// </summary>
		/// <param name="value">Output destination for the retrived value.</param>
		/// <param name="ty">The value type to retrive.</param>
		/// <returns>
		/// If true, the value was successfully retrived. Else, false; 
		/// and the output value should be ignored.
		/// </returns>
		virtual bool GetValue(int& value, ValTy ty = ValTy::Current) = 0;

		/// <summary>
		/// Set a Param value to a float.
		/// </summary>
		/// <param name="value">The value to set.</param>
		/// <param name="ty"></param>
		/// <returns>If true, the value was successfully set. Else, false</returns>
		virtual SetRet SetValue(float value, ValTy ty = ValTy::Current) = 0;

		/// <summary>
		/// Get a Param value as a float.
		/// </summary>
		/// <param name="value">Output destination for the retrived value.</param>
		/// <param name="ty">The value type to retrive.</param>
		/// <returns>
		/// If true, the value was successfully retrived. Else, false; 
		/// and the output value should be ignored.
		/// </returns>
		virtual bool GetValue(float& value, ValTy ty = ValTy::Current) = 0;

		/// <summary>
		/// Set a Param value to a string.
		/// </summary>
		/// <param name="value">The value to set.</param>
		/// <param name="ty"></param>
		/// <returns>If true, the value was successfully set. Else, false</returns>
		virtual SetRet SetValue(const std::string& value, ValTy ty = ValTy::Current) = 0;

		/// <summary>
		/// Get a Param value as a string.
		/// </summary>
		/// <param name="value">Output destination for the retrived value.</param>
		/// <param name="ty">The value type to retrive.</param>
		/// <returns>
		/// If true, the value was successfully retrived. Else, false; 
		/// and the output value should be ignored.
		/// </returns>
		virtual bool GetValue(std::string& value, ValTy ty = ValTy::Current) = 0;

		/// <summary>
		/// Set a Param value to a bool.
		/// </summary>
		/// <param name="value">The value to set.</param>
		/// <param name="ty"></param>
		/// <returns>If true, the value was successfully set. Else, false</returns>
		virtual SetRet SetValue(bool value, ValTy ty = ValTy::Current) = 0;

		/// <summary>
		/// Get a Param value as an bool.
		/// </summary>
		/// <param name="value">Output destination for the retrived value.</param>
		/// <param name="ty">The value type to retrive.</param>
		/// <returns>
		/// If true, the value was successfully retrived. Else, false; 
		/// and the output value should be ignored.
		/// </returns>
		virtual bool GetValue(bool& value, ValTy ty = ValTy::Current) = 0;

		/// <summary>
		/// Set the value of the Param from a json value.
		/// 
		/// </summary>
		/// <param name="value">
		/// The json data to set the Param to. It is expected to be a JSON
		/// representing either a bool, int, float, or string.
		/// </param>
		/// <param name="ty">The value type to set</param>
		/// <returns></returns>
		virtual SetRet SetValue(const json& value, ValTy ty = ValTy::Current);

		/// <summary>
		/// Reset the Param to its default value.
		/// </summary>
		/// <returns>
		/// True if the Param was reset. False if the Param does not
		/// have a default value.
		/// </returns>
		virtual SetRet ResetToDefault() = 0;

		/// <summary>
		/// Create a deep copy of the Param.
		/// </summary>
		/// <returns>
		/// Another Param of the same id, type, value and metadata.
		/// </returns>
		virtual ParamSPtr Clone() const = 0;

		/// <summary>
		/// Get the list of possible string options.
		/// </summary>
		/// <returns>
		/// The list of possible string options, only relevant for enums.
		/// </returns>
		virtual std::vector<std::string> GetPossible() const;

		/// <summary>
		/// Get a detailed description of the Param (including metadata)
		/// as a JSON object.
		/// 
		/// This will be the same JSON seen in the /Equipment queries.
		/// </summary>
		/// <returns>The Param details.</returns>
		json GetJSONDef();

		/// <summary>
		/// Get the Param's current value as a JSON object.
		/// </summary>
		/// <returns>The current value.</returns>
		json GetValueJSON();


	public:
		// Conversion conventions. 
		//////////////////////////////////////////////////
		// These functions should be used instead of doing is by 
		// hand to make sure all conversions involving Param data 
		// (involving bools) is consistent.
		//
		// They should all be compact enough to be inline functions

		inline static int BoolToInt(bool b)
		{
			return b ? 1 : 0;
		}

		inline static bool IntToBool(int i)
		{
			return i != 0;
		}

		inline static float BoolToFloat(bool b)
		{
			return b ? 1.0f : 0.0f;
		}

		inline static bool FloatToBool(float f)
		{
			return f != 0.0f;
		}

		inline static std::string BoolToString(bool b)
		{
			return b ? "True" : "False";
		}

		inline static bool StringToBool(const std::string& s)
		{
			return s.length() > 0 && s != "False";
		}
	};
}
