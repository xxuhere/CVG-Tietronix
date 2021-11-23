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

	// Event callback when the user presses the "Enter" key in the 
	// text input field.
	void OnTextEnter(wxCommandEvent& evt);

	// Event callback when the input field changes keyboard focus.
	// Specifically when the input field looses keyboard focus.
	void OnFocusChange(wxFocusEvent& evt);

	void OnDestroy(wxWindowDestroyEvent& evt);

	wxDECLARE_EVENT_TABLE();
};