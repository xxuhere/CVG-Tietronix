#include "DashEleCheckbox.h"
#include "../CVGBridge.h"

BEGIN_EVENT_TABLE(DashEleCheckbox, wxCheckBox)
	EVT_CHECKBOX(-1, DashEleCheckbox::OnValueChanged)
END_EVENT_TABLE()

DashEleCheckbox::DashEleCheckbox(wxWindow* parent, DashboardElementInst* eleInst)
	:	IDashEle(eleInst),
		wxCheckBox(parent, -1, "")
{
	this->Reattach();
}

bool DashEleCheckbox::Reattach()
{
	CVG::ParamSPtr param = this->Param();
	if(param == nullptr)
	{
		this->Disable();
	}
	else
	{
		this->Enable();
		this->OnParamValueChanged();
	}
	return true;
}

std::string DashEleCheckbox::ImplName()
{
	return DASHELENAME_DEFBOOL;
}

void DashEleCheckbox::Layout(const wxPoint& pixelPt, const wxSize& pixelSz, int cellSize)
{
	this->SetSize(wxRect(pixelPt, pixelSz));
}

void DashEleCheckbox::DestroyWindow()
{
	this->Destroy();
}

void DashEleCheckbox::OnParamValueChanged()
{
	CVG::ParamSPtr param = this->Param();
	if(param != nullptr)
	{ 
		bool b;
		if(param->GetValue(b))
			this->SetValue(b);
	}
}

void DashEleCheckbox::DrawPreview(wxPaintDC& dc, const wxPoint& offset)
{
	dc.SetPen(*wxCYAN_PEN);
	dc.SetBrush(*wxWHITE_BRUSH);

	wxRect r = this->GetRect();
	dc.DrawRectangle(r);
}

void DashEleCheckbox::Toggle(bool show)
{
	this->Show(show);
}

void DashEleCheckbox::OnValueChanged(wxCommandEvent& evt)
{
	CVG::ParamSPtr param = this->Param();
	if(param != nullptr)
	{
		bool b = this->GetValue();
		this->Bridge()->CVGB_SetValue(
			this->eleInst->EqGUID(), 
			param->GetID(), 
			CVG::Param::BoolToString(this->GetValue()));
	}
}