#include "ErrorTy.h"

namespace CVG
{
	ErrorTy ConvertToErrorTy(const std::string& str)
	{
		if (str == "log")
			return ErrorTy::Log;
		if (str == "warning")
			return ErrorTy::Warning;
		if (str == "error")
			return ErrorTy::Error;
		if (str == "fatal")
			return ErrorTy::Fatal;

		// Not quite sure how to handle this situation.
		// For now, unknown strings default to error.
		return ErrorTy::Error;
	}

	std::string ConvertToString(ErrorTy dt)
	{
		switch (dt)
		{
		case ErrorTy::Log:
			return "log";
		case ErrorTy::Warning:
			return "warning";
		case ErrorTy::Fatal:
			return "fatal";
		case ErrorTy::Error:
		default:
			return "error";
		}
	}
}