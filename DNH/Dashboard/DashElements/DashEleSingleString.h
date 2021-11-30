#pragma once
#include "IDashEle.h"

class DashEleSingleString : 
	public IDashEle,
	public wxTextCtrl
{
public:
	DashEleSingleString(wxWindow* parent, DashboardElementInst* eleInst);

protected:
	bool Reattach() override;
	std::string ImplName() override;
	void Layout(const wxPoint& pixelPt, const wxSize& pixelSz, int cellSize) override;
	void DestroyWindow() override;
	void OnParamValueChanged() override;
	void DrawPreview(wxPaintDC& dc, const wxPoint& offset) override;
	void Toggle(bool show) override;

	// Event callback when the "Enter" key is pressed.
	void OnInputEnter(wxCommandEvent& evt);

	// Event callback when the input changes keyboard focus.
	// Speficially, we're tracking when it looses keyboard focus.
	void OnInputFocus(wxFocusEvent& evt);

	DECLARE_EVENT_TABLE();
};