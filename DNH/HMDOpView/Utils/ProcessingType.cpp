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

	return "other";//This should never happen, maybe this is a failed assert?
}

ProcessingType StringToProcessingType(const std::string& str)
{
	if (str == "yen_threshold")
		return ProcessingType::yen_threshold;

	if (str == "yen_threshold_compressed")
		return ProcessingType::yen_threshold_compressed;

	if (str == "None")
		return ProcessingType::None;

	/// default, TODO fix this one
	return ProcessingType::None;;
}