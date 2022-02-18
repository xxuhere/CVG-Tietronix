#include "DashParamUIImplCheckbox.h"
#include "../CVGBridge.h"

BEGIN_EVENT_TABLE(DashParamUIImplCheckbox, wxCheckBox)
	EVT_CHECKBOX(-1, DashParamUIImplCheckbox::OnValueChanged)
END_EVENT_TABLE()

DashParamUIImplCheckbox::DashParamUIImplCheckbox(wxWindow* parent, DashboardElementInst* eleInst)
	:	DashParamUIImpl(eleInst),
		wxCheckBox(parent, -1, "")
{
	this->Reattach();
}

bool DashParamUIImplCheckbox::Reattach()
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

std::string DashParamUIImplCheckbox::ImplName()
{
	return DASHELENAME_DEFBOOL;
}

void DashParamUIImplCheckbox::Layout(const wxPoint& pixelPt, const wxSize& pixelSz, int cellSize)
{
	this->SetSize(wxRect(pixelPt, pixelSz));
}

void DashParamUIImplCheckbox::DestroyWindow()
{
	this->Destroy();
}

void DashParamUIImplCheckbox::OnParamValueChanged()
{
	CVG::ParamSPtr param = this->Param();
	if(param != nullptr)
	{ 
		bool b;
		if(param->GetValue(b))
			this->SetValue(b);
	}
}

void DashParamUIImplCheckbox::DrawPreview(wxPaintDC& dc, const wxPoint& offset)
{
	dc.SetPen(*wxCYAN_PEN);
	dc.SetBrush(*wxWHITE_BRUSH);

	wxRect r = this->GetRect();
	dc.DrawRectangle(r);
}

void DashParamUIImplCheckbox::Toggle(bool show)
{
	this->Show(show);
}

void DashParamUIImplCheckbox::OnValueChanged(wxCommandEvent& evt)
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