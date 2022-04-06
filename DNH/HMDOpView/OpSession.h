#pragma once
#include <string>

struct OpSession
{
	std::string patNameFirst;
	std::string patNameMid;
	std::string patNameLast;
	std::string sessionName;

	void SetName(
		const std::string& first, 
		const std::string& mid, 
		const std::string& last);

	void SetSession(const std::string& session);
};