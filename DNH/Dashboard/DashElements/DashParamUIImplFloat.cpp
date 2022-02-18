#include "DashParamUIImplFloat.h"
#include "../CVGBridge.h"

BEGIN_EVENT_TABLE(DashParamUIImplFloat, wxSpinCtrlDouble)
	EVT_SPINCTRLDOUBLE(-1, DashParamUIImplFloat::OnSpin)
END_EVENT_TABLE()

DashParamUIImplFloat::DashParamUIImplFloat(wxWindow* parent, DashboardElementInst* eleInst)
	:	wxSpinCtrlDouble(parent, -1),
		DashParamUIImpl(eleInst)
{
	this->Reattach();
}

bool DashParamUIImplFloat::Reattach()
{
	CVG::ParamSPtr param = this->Param();

	float fmax = std::numeric_limits<float>::max();
	float fmin = std::numeric_limits<float>::lowest();

	if(
		param == nullptr || 
		(
			// While not meant for ints, we won't explicitly exclude them.
			param->Type() != CVG::DataType::Float && 
			param->Type() != CVG::DataType::Int)
		)
	{
		this->SetRange(fmin, fmax);
		this->Disable();
		this->SetValue(0);
	}
	else
	{
		this->Enable();
		this->OnParamValueChanged();

		if(param->HasMin())
			param->GetValue(fmin, CVG::Param::ValTy::Min);

		if(param->HasMax())
			param->GetValue(fmax, CVG::Param::ValTy::Max);

		this->SetRange(fmin, fmax);
		this->OnParamValueChanged();
	}
	return true;
}

std::string DashParamUIImplFloat::ImplName()
{
	return DASHELENAME_DEFFLOAT;
}

void DashParamUIImplFloat::Layout(const wxPoint& pixelPt, const wxSize& pixelSz, int cellSize)
{
	this->SetSize(wxRect(pixelPt, pixelSz));
}

void DashParamUIImplFloat::DestroyWindow()
{
	this->Destroy();
}

void DashParamUIImplFloat::OnParamValueChanged()
{
	CVG::ParamSPtr param = this->Param();
	float fval;
	if(param->GetValue(fval) == true)
		this->SetValue(fval);
}

void DashParamUIImplFloat::DrawPreview(wxPaintDC& dc, const wxPoint& offset)
{
	dc.SetPen(*wxCYAN_PEN);
	dc.SetBrush(*wxWHITE_BRUSH);

	wxRect r = this->GetRect();
	dc.DrawRectangle(r);
}

void DashParamUIImplFloat::Toggle(bool show)
{
	this->Show(show);
}

void DashParamUIImplFloat::OnSpin(wxSpinDoubleEvent& evt)
{
	CVG::ParamSPtr param = this->Param();
	if(param == nullptr)
		return;

	this->Bridge()->CVGB_SetValue(
		this->eleInst->EqGUID(), 
		param->GetID(),
		std::to_string((float)this->GetValue()));
}