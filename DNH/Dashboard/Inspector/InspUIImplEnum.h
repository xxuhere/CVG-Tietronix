#pragma once
#include "InspUIImplParam.h"
#include <Params/Param.h>

/// <summary>
/// Default implementation of a InspUIImplParam for enum Params.
/// </summary>
class InspUIImplEnum : 
	public InspUIImplParam,
	public wxComboBox
{
private:
	bool destroyed = false;

private:
	static wxArrayString CreateOptionsString(CVG::ParamSPtr p);
public:
	InspUIImplEnum(InspBarParam* owner, CVGBridge* bridge);

	void UpdateDisplay() override;
	void DestroyUI() override;
	wxWindow* GetWindow() override;

	// Event callback for when the user modifies the combobox selection.
	void OnValueChanged(wxCommandEvent& evt);

	void OnDestroy(wxWindowDestroyEvent& evt);
	wxDECLARE_EVENT_TABLE();
};