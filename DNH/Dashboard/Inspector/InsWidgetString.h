#pragma once
#include "InsWidgetParam.h"

/// <summary>
/// Default implementation of a InsWidgetParam for string Params.
/// </summary>
class InsWidgetString : 
	public InsWidgetParam, 
	public wxTextCtrl
{
private:
	bool destroyed = false;

public:
	InsWidgetString(InspectorParam* owner, CVGBridge* bridge);

	void UpdateDisplay() override;
	void DestroyUI() override;
	wxWindow* GetWindow() override;

	void OnTextEnter(wxCommandEvent& evt);
	void OnFocusChange(wxFocusEvent& evt);
	void OnDestroy(wxWindowDestroyEvent& evt);

	wxDECLARE_EVENT_TABLE();
};