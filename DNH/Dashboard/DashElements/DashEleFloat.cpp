#include "DashEleFloat.h"
#include "../CVGBridge.h"

BEGIN_EVENT_TABLE(DashEleFloat, wxSpinCtrlDouble)
	EVT_SPINCTRLDOUBLE(-1, DashEleFloat::OnSpin)
END_EVENT_TABLE()

DashEleFloat::DashEleFloat(wxWindow* parent, DashboardElementInst* eleInst)
	:	wxSpinCtrlDouble(parent, -1),
		IDashEle(eleInst)
{
	this->Reattach();
}

bool DashEleFloat::Reattach()
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

std::string DashEleFloat::ImplName()
{
	return DASHELENAME_DEFFLOAT;
}

void DashEleFloat::Layout(const wxPoint& pixelPt, const wxSize& pixelSz, int cellSize)
{
	this->SetSize(wxRect(pixelPt, pixelSz));
}

void DashEleFloat::DestroyWindow()
{
	this->Destroy();
}

void DashEleFloat::OnParamValueChanged()
{
	CVG::ParamSPtr param = this->Param();
	float fval;
	if(param->GetValue(fval) == true)
		this->SetValue(fval);
}

void DashEleFloat::OnSpin(wxSpinDoubleEvent& evt)
{
	CVG::ParamSPtr param = this->Param();
	if(param == nullptr)
		return;

	this->Bridge()->CVGB_SetValue(
		this->eleInst->EqGUID(), 
		param->GetID(),
		std::to_string((float)this->GetValue()));
}