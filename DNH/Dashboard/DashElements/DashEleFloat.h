#pragma once
#include "IDashEle.h"
#include <wx/spinctrl.h>

/// <summary>
/// Default dashboard UI implementation for float types.
/// </summary>
class DashEleFloat : 
	public IDashEle,
	public wxSpinCtrlDouble
{
public:
	DashEleFloat(wxWindow* parent, DashboardElementInst* eleInst);

protected:
	bool Reattach() override;
	std::string ImplName() override;
	void Layout(const wxPoint& pixelPt, const wxSize& pixelSz, int cellSize) override;
	void DestroyWindow() override;
	void OnParamValueChanged() override;

	// Event handler for when the user changes the spinctrl value.
	void OnSpin(wxSpinDoubleEvent& evt);

	DECLARE_EVENT_TABLE()

};