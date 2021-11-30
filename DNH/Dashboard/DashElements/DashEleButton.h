#pragma once
#include "IDashEle.h"

/// <summary>
/// Default dashboard UI implementation for event types.
/// </summary>
class DashEleButton:
	public IDashEle,
	public wxButton
{
public:
	DashEleButton(wxWindow* parent, DashboardElementInst* eleInst);

protected:
	bool Reattach() override;
	std::string ImplName() override;
	void Layout(const wxPoint& pixelPt, const wxSize& pixelSz, int cellSize) override;
	void DestroyWindow() override;
	void OnParamValueChanged() override;
	void DrawPreview(wxPaintDC& dc, const wxPoint& offset) override;
	void Toggle(bool show) override;

	// Event handler for when the user changes the spinctrl value.
	void OnButton(wxCommandEvent& evt);

	DECLARE_EVENT_TABLE()
};