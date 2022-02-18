#pragma once
#include "InspUIImplParam.h"

// Default implementation of a InspUIImplParam for an event Param
class InspUIImplButton : 
	public InspUIImplParam,
	public wxButton
{
private:

public:
	InspUIImplButton(InspBarParam* owner, CVGBridge* bridge);

	void UpdateDisplay() override;
	void DestroyUI() override;
	wxWindow* GetWindow() override;

	void OnButtonPress(wxCommandEvent& evt);

	wxDECLARE_EVENT_TABLE();
};