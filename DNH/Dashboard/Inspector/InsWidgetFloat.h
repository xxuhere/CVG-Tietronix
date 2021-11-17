#pragma once
#include "InsWidgetParam.h"
#include <wx/spinctrl.h>

/// <summary>
/// Default implementation of a InsWidgetParam for float Params.
/// </summary>
class InsWidgetFloat : 
	public InsWidgetParam,
	public wxSpinCtrlDouble
{
private:
	bool destroyed = false;

public:

	InsWidgetFloat(InspectorParam* owner, CVGBridge* bridge);

	void UpdateDisplay() override;
	void DestroyUI() override;
	wxWindow* GetWindow() override;

	void OnTextEnter(wxCommandEvent& evt);
	void OnFocusChange(wxFocusEvent& evt);
	void OnValChanged(wxSpinDoubleEvent& evt);
	void OnDestroy(wxWindowDestroyEvent& evt);

	wxDECLARE_EVENT_TABLE();
};