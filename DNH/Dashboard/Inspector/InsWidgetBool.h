#pragma once
#include "InsWidgetParam.h"

/// <summary>
/// Default implementation of a InsWidgetParam for bool Params.
/// </summary>
class InsWidgetBool : 
	public InsWidgetParam,
	public wxCheckBox
{
private:
	/// <summary>
	/// If true, the UI has been destroyed.
	/// </summary>
	bool destroyed = false;

public:

	InsWidgetBool(InspectorParam* owner, CVGBridge* bridge);

	void UpdateDisplay() override;
	void DestroyUI() override;
	wxWindow* GetWindow() override;

	void OnCheckbox(wxCommandEvent& evt);
	void OnDestroy(wxWindowDestroyEvent& evt);

	wxDECLARE_EVENT_TABLE();
};