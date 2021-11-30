#include "DashEleButton.h"
#include "../CVGBridge.h"

BEGIN_EVENT_TABLE(DashEleButton, wxButton)
	EVT_BUTTON(-1, DashEleButton::OnButton)
END_EVENT_TABLE()

DashEleButton::DashEleButton(wxWindow* parent, DashboardElementInst* eleInst)
	:	IDashEle(eleInst),
		wxButton(parent, -1, "")
{
	this->Reattach();
}

bool DashEleButton::Reattach()
{
	CVG::ParamSPtr param = this->Param();
	if(param == nullptr)
	{
		this->SetLabel("");
		return false;
	}

	std::string label = param->GetLabel();
	this->SetLabel(label);
	return true;
}

std::string DashEleButton::ImplName()
{
	return DASHELENAME_DEFEVENT;
}

void DashEleButton::Layout(const wxPoint& pixelPt, const wxSize& pixelSz, int cellSize)
{
	this->SetSize(wxRect(pixelPt, pixelSz));
}

void DashEleButton::DestroyWindow()
{
	this->Destroy();
}

void DashEleButton::OnParamValueChanged()
{
	// Do nothing
}

void DashEleButton::DrawPreview(wxPaintDC& dc, const wxPoint& offset)
{
	dc.SetPen(*wxCYAN_PEN);
	dc.SetBrush(*wxWHITE_BRUSH);

	wxRect r = this->GetRect();
	dc.DrawRectangle(r);
}

void DashEleButton::Toggle(bool show)
{
	this->Show(show);
}

// Event handler for when the user changes the spinctrl value.
void DashEleButton::OnButton(wxCommandEvent& evt)
{
	CVG::ParamSPtr param = this->eleInst->Param();
	if(param == nullptr)
		return;

	this->Bridge()->CVGB_Submit(
		this->eleInst->EqGUID(),
		param->GetID());
}