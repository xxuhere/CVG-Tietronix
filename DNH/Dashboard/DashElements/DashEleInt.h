#pragma once
#include "IDashEle.h"
#include <wx/spinctrl.h>

/// <summary>
/// Default dashboard UI implementation for int types.
/// </summary>
class DashEleInt : 
	public IDashEle,
	public wxSpinCtrl
{
public:
	DashEleInt(wxWindow* parent, DashboardElementInst* eleInst);

protected:
	bool Reattach() override;
	std::string ImplName() override;
	void Layout(const wxPoint& pixelPt, const wxSize& pixelSz, int cellSize) override;
	void DestroyWindow() override;
	void OnParamValueChanged() override;
	void DrawPreview(wxPaintDC& dc, const wxPoint& offset) override;
	void Toggle(bool show) override;

	// Event handler for when the user changes the spinctrl value.
	void OnSpin(wxSpinEvent& evt);

	DECLARE_EVENT_TABLE()
};