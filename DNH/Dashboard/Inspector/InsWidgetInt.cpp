#include "InsWidgetInt.h"
#include "InspectorParam.h"
#include <limits>

BEGIN_EVENT_TABLE(InsWidgetInt, wxSpinCtrl)
	EVT_WINDOW_DESTROY(InsWidgetInt::OnDestroy)
	EVT_SPINCTRL(-1, InsWidgetInt::OnValChanged)
	EVT_TEXT_ENTER(-1, InsWidgetInt::OnTextEnter)
	EVT_SET_FOCUS(InsWidgetInt::OnFocusChange)
END_EVENT_TABLE()

InsWidgetInt::InsWidgetInt(InspectorParam* owner, CVGBridge* bridge)
	:	InsWidgetParam(owner, bridge),
		wxSpinCtrl(owner, -1, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER)
{
	CVG::ParamSPtr p = owner->param;

	int i;
	int iMin = std::numeric_limits<int>::lowest();
	int iMax = std::numeric_limits<int>::max();
	if(p->GetValue(i, CVG::Param::ValTy::Min))
		iMin = (int)i;
	if(p->GetValue(i, CVG::Param::ValTy::Max))
		iMax = (int)i;

	this->SetRange(iMin, iMax);

	this->UpdateDisplay();
}

void InsWidgetInt::UpdateDisplay()
{
	CVG::ParamSPtr p = this->parent->param;

	int i;
	p->GetValue(i);
	this->SetValue(i);
}

void InsWidgetInt::DestroyUI()
{
	if(this->destroyed)
		return;

	this->Destroy();
}

wxWindow* InsWidgetInt::GetWindow()
{
	return 
		this->destroyed ?
			nullptr:
			this;
}

void InsWidgetInt::OnValChanged(wxSpinEvent& evt)
{
	std::string asVal = std::to_string(this->GetValue());
	this->SendParamValueToServer(asVal);
}

void InsWidgetInt::OnTextEnter(wxCommandEvent& evt)
{
	std::string asVal = std::to_string(this->GetValue());
	this->SendParamValueToServer(asVal);
}

void InsWidgetInt::OnFocusChange(wxFocusEvent& evt)
{
	evt.Skip();

	if(evt.GetWindow() == this)
	{
		std::string asVal = std::to_string(this->GetValue());
		this->SendParamValueToServer(asVal);
	}
}

void InsWidgetInt::OnDestroy(wxWindowDestroyEvent& evt)
{
	this->destroyed = true;
}