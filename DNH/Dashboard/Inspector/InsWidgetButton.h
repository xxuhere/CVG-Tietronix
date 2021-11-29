#pragma once
#include "InsWidgetParam.h"

// Default implementation of a InsWidgetParam for an event Param
class InsWidgetButton : 
	public InsWidgetParam,
	public wxButton
{
private:

public:
	InsWidgetButton(InspectorParam* owner, CVGBridge* bridge);

	void UpdateDisplay() override;
	void DestroyUI() override;
	wxWindow* GetWindow() override;

	void OnButtonPress(wxCommandEvent& evt);

	wxDECLARE_EVENT_TABLE();
};