#pragma once
#include "InsWidgetParam.h"
#include <wx/spinctrl.h>

/// <summary>
/// Default implementation of a InsWidgetParam for int Params.
/// </summary>
class InsWidgetInt : 
	public InsWidgetParam,
	public wxSpinCtrl
{
private:
	bool destroyed = false;

public:

	InsWidgetInt(InspectorParam* owner, CVGBridge* bridge);

	void UpdateDisplay() override;
	void DestroyUI() override;
	wxWindow* GetWindow() override;

	void OnTextEnter(wxCommandEvent& evt);
	void OnFocusChange(wxFocusEvent& evt);
	void OnValChanged(wxSpinEvent& evt);
	void OnDestroy(wxWindowDestroyEvent& evt);

	wxDECLARE_EVENT_TABLE();
};