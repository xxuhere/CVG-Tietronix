#pragma once
#include <string>

namespace CVG
{
	/// <summary>
	/// Various types of equipment
	/// </summary>
	enum class EQType
	{
		Lamp = 0,
		Camera,
		Sonic,
		Mic,
		Sensor,
		Cut,
		Input,
		Event,
		Actuator,
		Display,
		Spectator,
		Conductor,

		// Null should always be the second-to-last entry. It doubles as 
		// the counter for how many valid DataType values there are.
		Null,
		// Same value as Unknown, but more descriptive name, to give options 
		// to better describe why Totalnum or Unknown is being used.
		Totalnum = Null
	};

	/// <summary>
	/// Convert an Equipment type JSON string value to a C++ enum value.
	/// </summary>
	/// <param name="str">The API string from of an Equipment type.</param>
	/// <returns></returns>
	EQType ConvertToEqType(const std::string& str);

	/// <summary>
	/// Convert an Equipment type C++ enum value to a JSON string value.
	/// </summary>
	/// <param name="dt">The Equipment type to convert to an API string.</param>
	/// <returns>The enum converted to a string recognized by the API.</returns>
	std::string ConvertToString(EQType dt);
}