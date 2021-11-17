#include "InsWidgetFloat.h"
#include "InspectorParam.h"
#include <limits>

BEGIN_EVENT_TABLE(InsWidgetFloat, wxSpinCtrlDouble)
	EVT_WINDOW_DESTROY(InsWidgetFloat::OnDestroy)
	EVT_SPINCTRLDOUBLE(-1, InsWidgetFloat::OnValChanged)
	EVT_TEXT_ENTER(-1, InsWidgetFloat::OnTextEnter)
	EVT_SET_FOCUS(InsWidgetFloat::OnFocusChange)
END_EVENT_TABLE()

InsWidgetFloat::InsWidgetFloat(InspectorParam * owner, CVGBridge* bridge)
	:	InsWidgetParam(owner, bridge),
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

void InsWidgetFloat::UpdateDisplay()
{
	CVG::ParamSPtr p = this->parent->param;

	float f;
	p->GetValue(f);
	this->SetValue(f);
}

void InsWidgetFloat::DestroyUI()
{
	if(this->destroyed)
		return;

	this->Destroy();
}


wxWindow* InsWidgetFloat::GetWindow()
{
	return 
		this->destroyed ?
			nullptr:
			this;
}

void InsWidgetFloat::OnValChanged(wxSpinDoubleEvent& evt)
{
	std::string asVal = std::to_string(this->GetValue());
	this->SendParamValueToServer(asVal);
}

void InsWidgetFloat::OnTextEnter(wxCommandEvent& evt)
{
	std::string asVal = std::to_string(this->GetValue());
	this->SendParamValueToServer(asVal);
}

void InsWidgetFloat::OnFocusChange(wxFocusEvent& evt)
{
	evt.Skip();

	if(evt.GetWindow() == this)
	{
		std::string asVal = std::to_string(this->GetValue());
		this->SendParamValueToServer(asVal);
	}
}

void InsWidgetFloat::OnDestroy(wxWindowDestroyEvent& evt)
{
	this->destroyed = true;
}