#pragma once
#include "InspUIImplParam.h"

/// <summary>
/// Default implementation of a InspUIImplParam for bool Params.
/// </summary>
class InspUIImplBool : 
	public InspUIImplParam,
	public wxCheckBox
{
private:
	/// <summary>
	/// If true, the UI has been destroyed.
	/// </summary>
	bool destroyed = false;

public:

	InspUIImplBool(InspBarParam* owner, CVGBridge* bridge);

	void UpdateDisplay() override;
	void DestroyUI() override;
	wxWindow* GetWindow() override;

	// Event callback for when the user modifies the checkbox.
	void OnCheckbox(wxCommandEvent& evt);

	void OnDestroy(wxWindowDestroyEvent& evt);

	wxDECLARE_EVENT_TABLE();
};