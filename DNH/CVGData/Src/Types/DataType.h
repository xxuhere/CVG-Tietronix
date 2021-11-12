#pragma once
#include <string>

namespace CVG {
	/// <summary>
	/// Identifiers for various datatypes in the CVG JSON API.
	/// </summary>
	enum class DataType
	{
		// The Data is a string. When used in the Param system, 
		// a String is implemented with the ParamString subclass.
		/// <see cref="ParamString"/>
		String = 0,

		// The Data is an int. When used in the Param system,
		// an Int is implemented with the ParamInt subclass.
		/// <see cref="ParamInt"/>
		Int,

		// The Data is a float. When used in the Param system,
		// a Float is implemented with the ParamFloat subclass.
		/// <see cref="ParamFloat"/>
		Float,

		// The Data is a bool. When used in the Param system,
		// a Bool is implemented with the ParamBool subclass.
		/// <see cref="ParamBool"/>
		Bool,

		// The Data is an enum. When used in the Param system,
		// an Enum is implemented with the ParamEnum subclass.
		/// <see cref="ParamBool"/>
		Enum,

		// Unknown should always be the second-to-last entry. It doubles as 
		// the counter for how many valid DataType values there are.
		Unknown, 
		// Same value as Unknown, but more descriptive name, to give options 
		// to better describe why Totalnum or Unknown is being used.
		Totalnum = Unknown
	};

	/// <summary>
	/// Convert a DataType API string value to a C++ enum value.
	/// </summary>
	/// <param name="str">The API string form of a DataType.</param>
	/// <returns>The converted C++ DataType enum.</returns>
	DataType ConvertToDataType(std::string str);

	/// <summary>
	/// Convert a DataType C++ enum value to a JSON string value.
	/// </summary>
	/// <param name="dt">The DataType to convert to aPI string.</param>
	/// <returns>The enum converted to a string recognized by the API.</returns>
	std::string ConvertToString(DataType dt);
}

