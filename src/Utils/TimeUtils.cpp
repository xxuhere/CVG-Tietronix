#include "TimeUtils.h"
#include <wx/datetime.h>
#include <ostream>
#include <iomanip>
#include <sstream>

std::string FileDateTimeNow()
{
	wxDateTime dt = wxDateTime::Now();
	std::stringstream retStream;

	retStream << 
		dt.GetYear() << 
		std::setfill('0') << std::setw(2) << (dt.GetMonth() + 1) << 
		std::setfill('0') << std::setw(2) << dt.GetDay() << 
		std::setfill('0') << std::setw(2) << dt.GetHour() << 
		std::setfill('0') << std::setw(2) << dt.GetMinute() << 
		std::setfill('0') << std::setw(2) << dt.GetSecond();

	return retStream.str();
}