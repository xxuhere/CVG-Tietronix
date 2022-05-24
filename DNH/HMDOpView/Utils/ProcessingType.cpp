#include "ProcessingType.h"

std::string to_string(ProcessingType ty)
{
	switch (ty)
	{

	case ProcessingType::None:
		return "None";

	case ProcessingType::yen_threshold:
		return "yen_threshold";

	case ProcessingType::yen_threshold_compressed:
		return "yen_threshold_compressed";
	}

	//This should never happen, maybe this needs an illegal location assert?
	return "None";
}

ProcessingType StringToProcessingType(const std::string& str)
{
	if (str == "yen_threshold")
		return ProcessingType::yen_threshold;

	if (str == "yen_threshold_compressed")
		return ProcessingType::yen_threshold_compressed;

	//if (str == "None")
	return ProcessingType::None;
}