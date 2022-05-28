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

	case ProcessingType::static_threshold:
		return "static_threshold";

	case ProcessingType::two_stdev_from_mean:
		return "two_stdev_from_mean";
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

	if (str == "static_threshold")
		return ProcessingType::static_threshold;

	if (str == "two_stdev_from_mean")
		return ProcessingType::two_stdev_from_mean;

	//if (str == "None")
	return ProcessingType::None;
}