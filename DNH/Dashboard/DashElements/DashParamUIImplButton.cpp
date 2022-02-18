#include "DashParamUIImplButton.h"
#include "../CVGBridge.h"

BEGIN_EVENT_TABLE(DashParamUIImplButton, wxButton)
	EVT_BUTTON(-1, DashParamUIImplButton::OnButton)
END_EVENT_TABLE()

DashParamUIImplButton::DashParamUIImplButton(wxWindow* parent, DashboardElementInst* eleInst)
	:	DashParamUIImpl(eleInst),
		wxButton(parent, -1, "")
{
	this->Reattach();
}

bool DashParamUIImplButton::Reattach()
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

std::string DashParamUIImplButton::ImplName()
{
	return DASHELENAME_DEFEVENT;
}

void DashParamUIImplButton::Layout(const wxPoint& pixelPt, const wxSize& pixelSz, int cellSize)
{
	this->SetSize(wxRect(pixelPt, pixelSz));
}

void DashParamUIImplButton::DestroyWindow()
{
	this->Destroy();
}

void DashParamUIImplButton::OnParamValueChanged()
{
	// Do nothing
}

void DashParamUIImplButton::DrawPreview(wxPaintDC& dc, const wxPoint& offset)
{
	dc.SetPen(*wxCYAN_PEN);
	dc.SetBrush(*wxWHITE_BRUSH);

	wxRect r = this->GetRect();
	dc.DrawRectangle(r);
}

void DashParamUIImplButton::Toggle(bool show)
{
	this->Show(show);
}

// Event handler for when the user changes the spinctrl value.
void DashParamUIImplButton::OnButton(wxCommandEvent& evt)
{
	CVG::ParamSPtr param = this->eleInst->Param();
	if(param == nullptr)
		return;

	this->Bridge()->CVGB_Submit(
		this->eleInst->EqGUID(),
		param->GetID());
}