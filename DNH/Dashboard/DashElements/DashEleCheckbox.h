#pragma once
#include "IDashEle.h"
#include <wx/checkbox.h>


class DashEleCheckbox :
	public IDashEle,
	public wxCheckBox
{
public:
	DashEleCheckbox(wxWindow* parent, DashboardElementInst* eleInst);

protected:
	bool Reattach() override;
	std::string ImplName() override;
	void Layout(const wxPoint& pixelPt, const wxSize& pixelSz, int cellSize) override;
	void DestroyWindow() override;
	void OnParamValueChanged() override;

	void OnValueChanged(wxCommandEvent& evt);

	DECLARE_EVENT_TABLE();
};