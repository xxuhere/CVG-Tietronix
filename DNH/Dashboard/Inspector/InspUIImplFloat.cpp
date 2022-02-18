#include "InspUIImplFloat.h"
#include "InspBarParam.h"
#include <limits>

BEGIN_EVENT_TABLE(InspUIImplFloat, wxSpinCtrlDouble)
	EVT_WINDOW_DESTROY(InspUIImplFloat::OnDestroy)
	EVT_SPINCTRLDOUBLE(-1, InspUIImplFloat::OnValChanged)
	EVT_TEXT_ENTER(-1, InspUIImplFloat::OnTextEnter)
	EVT_SET_FOCUS(InspUIImplFloat::OnFocusChange)
END_EVENT_TABLE()

InspUIImplFloat::InspUIImplFloat(InspBarParam * owner, CVGBridge* bridge)
	:	InspUIImplParam(owner, bridge),
		wxSpinCtrlDouble(owner, -1)
{
	CVG::ParamSPtr p = owner->param;
	
	float f;
	double dMin = std::numeric_limits<double>::lowest();
	double dMax = std::numeric_limits<double>::max();
	if(p->GetValue(f, CVG::Param::ValTy::Min))
		dMin = (double)f;

	if(p->GetValue(f, CVG::Param::ValTy::Max))
		dMax = (double)f;

	this->SetRange(dMin, dMax);

	this->UpdateDisplay();
}

void InspUIImplFloat::UpdateDisplay()
{
	CVG::ParamSPtr p = this->parent->param;

	float f;
	p->GetValue(f);
	this->SetValue(f);
}

void InspUIImplFloat::DestroyUI()
{
	if(this->destroyed)
		return;

	this->Destroy();
}


wxWindow* InspUIImplFloat::GetWindow()
{
	return 
		this->destroyed ?
			nullptr:
			this;
}

void InspUIImplFloat::OnValChanged(wxSpinDoubleEvent& evt)
{
	std::string asVal = std::to_string(this->GetValue());
	this->SendParamValueToServer(asVal);
}

void InspUIImplFloat::OnTextEnter(wxCommandEvent& evt)
{
	std::string asVal = std::to_string(this->GetValue());
	this->SendParamValueToServer(asVal);
}

void InspUIImplFloat::OnFocusChange(wxFocusEvent& evt)
{
	evt.Skip();

	if(evt.GetWindow() == this)
	{
		std::string asVal = std::to_string(this->GetValue());
		this->SendParamValueToServer(asVal);
	}
}

void InspUIImplFloat::OnDestroy(wxWindowDestroyEvent& evt)
{
	this->destroyed = true;
}