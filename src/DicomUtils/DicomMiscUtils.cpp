#include "DicomMiscUtils.h"

#if WIN32
	#include <windows.h>
	#include <sysinfoapi.h>
#endif

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <ctime>
#include <sstream>
#include <ostream>

namespace DicomMiscUtils
{ 
	std::string GetCurrentDateString()
	{
		boost::gregorian::date curDate(boost::gregorian::day_clock::local_day());

		std::stringstream sstrm;
		sstrm << 
			std::setw(4) << std::setfill('0') << curDate.year() << 
			std::setw(2) << std::setfill('0') << curDate.month().as_number() << // w\o as_number, it displays text name
			std::setw(2) << std::setfill('0') << curDate.day();

		return sstrm.str();
	}

	std::string GetCurrentTime()
	{
		boost::posix_time::ptime curTime = 
			boost::posix_time::second_clock::local_time();

		std::stringstream sstrm;
		sstrm <<
			std::setw(2) << std::setfill('0') << curTime.time_of_day().hours() <<
			std::setw(2) << std::setfill('0') << curTime.time_of_day().minutes() <<
			std::setw(2) << std::setfill('0') << curTime.time_of_day().seconds();

		return sstrm.str();

	}
}