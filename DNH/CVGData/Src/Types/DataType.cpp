#include "DataType.h"

namespace CVG
{
	DataType ConvertToDataType(std::string str)
	{
		if (str == "string")
			return DataType::String;
		if (str == "int")
			return DataType::Int;
		if (str == "bool")
			return DataType::Bool;
		if (str == "float")
			return DataType::Float;
		if (str == "enum")
			return DataType::Enum;

		return DataType::Unknown;
	}

	std::string ConvertToString(DataType dt)
	{
		switch (dt)
		{
		case DataType::String:
			return "string";

		case DataType::Int:
			return "int";

		case DataType::Float:
			return "float";

		case DataType::Bool:
			return "bool";

		case DataType::Enum:
			return "enum";
		}

		return "unknown";
	}
}