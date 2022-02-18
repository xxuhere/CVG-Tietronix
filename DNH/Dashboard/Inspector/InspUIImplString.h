#pragma once
#include "InspUIImplParam.h"

/// <summary>
/// Default implementation of a InspUIImplParam for string Params.
/// </summary>
class InspUIImplString : 
	public InspUIImplParam, 
	public wxTextCtrl
{
private:
	bool destroyed = false;

public:
	InspUIImplString(InspBarParam* owner, CVGBridge* bridge);

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