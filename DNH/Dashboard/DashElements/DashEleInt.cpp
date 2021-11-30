#include "DashEleInt.h"
#include "../CVGBridge.h"


BEGIN_EVENT_TABLE(DashEleInt, wxSpinCtrl)
	EVT_SPIN(-1, DashEleInt::OnSpin)
END_EVENT_TABLE()

#include "DashEleInt.h"

DashEleInt::DashEleInt(wxWindow* parent, DashboardElementInst* eleInst)
	:	wxSpinCtrl(parent, -1),
		IDashEle(eleInst)
{
}

bool DashEleInt::Reattach()
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

std::string DashEleInt::ImplName()
{
	return DASHELENAME_DEFINT;
}

void DashEleInt::Layout(const wxPoint& pixelPt, const wxSize& pixelSz, int cellSize)
{
	this->SetSize(wxRect(pixelPt, pixelSz));
}

void DashEleInt::DestroyWindow()
{
	this->Destroy();
}

void DashEleInt::OnParamValueChanged()
{
	CVG::ParamSPtr param = this->Param();
	float fval;
	if(param->GetValue(fval) == true)
		this->SetValue(fval);
}

void DashEleInt::DrawPreview(wxPaintDC& dc, const wxPoint& offset)
{
	dc.SetPen(*wxCYAN_PEN);
	dc.SetBrush(*wxWHITE_BRUSH);

	wxRect r = this->GetRect();
	dc.DrawRectangle(r);
}

void DashEleInt::Toggle(bool show)
{
	this->Show(show);
}

void DashEleInt::OnSpin(wxSpinEvent& evt)
{
	CVG::ParamSPtr param = this->Param();
	if(param == nullptr)
		return;

	this->Bridge()->CVGB_SetValue(
		this->eleInst->EqGUID(), 
		param->GetID(),
		std::to_string(this->GetValue()));
}