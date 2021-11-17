#pragma once
#include <string>
#include <wx/wx.h>

class DashboardGrid;

// NOT USED YET - Will be the container class for an
// item on the dashboard.
class DashboardElement
{
	wxPoint dashPos;
	wxSize dashSize;

	std::string guid;
	std::string paramID;
	std::string label;

	DashboardGrid* gridOwner = nullptr;
	wxWindow* uiImpl = nullptr;

public:
	inline const std::string& EqGUID() const
	{ return this->guid; }

	inline const std::string& ParamID() const
	{ return this->paramID; }

	bool DestroyUIImpl();
};