#include "DashParamUIImplInt.h"
#include "../CVGBridge.h"


BEGIN_EVENT_TABLE(DashParamUIImplInt, wxSpinCtrl)
	EVT_SPIN(-1, DashParamUIImplInt::OnSpin)
END_EVENT_TABLE()

#include "DashParamUIImplInt.h"

DashParamUIImplInt::DashParamUIImplInt(wxWindow* parent, DashboardElementInst* eleInst)
	:	wxSpinCtrl(parent, -1),
		DashParamUIImpl(eleInst)
{
}

bool DashParamUIImplInt::Reattach()
{
	CVG::ParamSPtr param = this->Param();

	int imax = std::numeric_limits<int>::max();
	int imin = std::numeric_limits<int>::lowest();

	if(
		param == nullptr || 
		(
			// While not meant for ints, we won't explicitly exclude them.
			param->Type() != CVG::DataType::Float && 
			param->Type() != CVG::DataType::Int)
		)
	{
		this->SetRange(imin, imax);
		this->Disable();
		this->SetValue(0);
	}
	else
	{
		this->Enable();
		this->OnParamValueChanged();

		if(param->HasMin())
			param->GetValue(imin, CVG::Param::ValTy::Min);

		if(param->HasMax())
			param->GetValue(imax, CVG::Param::ValTy::Max);

		this->SetRange(imin, imax);
		this->OnParamValueChanged();
	}
	return true;
}

std::string DashParamUIImplInt::ImplName()
{
	return DASHELENAME_DEFINT;
}

void DashParamUIImplInt::Layout(const wxPoint& pixelPt, const wxSize& pixelSz, int cellSize)
{
	this->SetSize(wxRect(pixelPt, pixelSz));
}

void DashParamUIImplInt::DestroyWindow()
{
	this->Destroy();
}

void DashParamUIImplInt::OnParamValueChanged()
{
	CVG::ParamSPtr param = this->Param();
	float fval;
	if(param->GetValue(fval) == true)
		this->SetValue(fval);
}

void DashParamUIImplInt::DrawPreview(wxPaintDC& dc, const wxPoint& offset)
{
	dc.SetPen(*wxCYAN_PEN);
	dc.SetBrush(*wxWHITE_BRUSH);

	wxRect r = this->GetRect();
	dc.DrawRectangle(r);
}

void DashParamUIImplInt::Toggle(bool show)
{
	this->Show(show);
}

void DashParamUIImplInt::OnSpin(wxSpinEvent& evt)
{
	CVG::ParamSPtr param = this->Param();
	if(param == nullptr)
		return;

	this->Bridge()->CVGB_SetValue(
		this->eleInst->EqGUID(), 
		param->GetID(),
		std::to_string(this->GetValue()));
}