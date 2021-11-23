#pragma once
#include "InsWidgetParam.h"
#include "Params/Param.h"

/// <summary>
/// Default implementation of a InsWidgetParam for enum Params.
/// </summary>
class InsWidgetEnum : 
	public InsWidgetParam,
	public wxComboBox
{
private:
	bool destroyed = false;

private:
	static wxArrayString CreateOptionsString(CVG::ParamSPtr p);
public:
	InsWidgetEnum(InspectorParam* owner, CVGBridge* bridge);

	void UpdateDisplay() override;
	void DestroyUI() override;
	wxWindow* GetWindow() override;

	// Event callback for when the user modifies the combobox selection.
	void OnValueChanged(wxCommandEvent& evt);

	void OnDestroy(wxWindowDestroyEvent& evt);
	wxDECLARE_EVENT_TABLE();
};