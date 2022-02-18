#include "InspUIImplInt.h"
#include "InspBarParam.h"
#include <limits>

BEGIN_EVENT_TABLE(InspUIImplInt, wxSpinCtrl)
	EVT_WINDOW_DESTROY(InspUIImplInt::OnDestroy)
	EVT_SPINCTRL(-1, InspUIImplInt::OnValChanged)
	EVT_TEXT_ENTER(-1, InspUIImplInt::OnTextEnter)
	EVT_SET_FOCUS(InspUIImplInt::OnFocusChange)
END_EVENT_TABLE()

InspUIImplInt::InspUIImplInt(InspBarParam* owner, CVGBridge* bridge)
	:	InspUIImplParam(owner, bridge),
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

void InspUIImplInt::UpdateDisplay()
{
	CVG::ParamSPtr p = this->parent->param;

	int i;
	p->GetValue(i);
	this->SetValue(i);
}

void InspUIImplInt::DestroyUI()
{
	if(this->destroyed)
		return;

	this->Destroy();
}

wxWindow* InspUIImplInt::GetWindow()
{
	return 
		this->destroyed ?
			nullptr:
			this;
}

void InspUIImplInt::OnValChanged(wxSpinEvent& evt)
{
	std::string asVal = std::to_string(this->GetValue());
	this->SendParamValueToServer(asVal);
}

void InspUIImplInt::OnTextEnter(wxCommandEvent& evt)
{
	std::string asVal = std::to_string(this->GetValue());
	this->SendParamValueToServer(asVal);
}

void InspUIImplInt::OnFocusChange(wxFocusEvent& evt)
{
	evt.Skip();

	if(evt.GetWindow() == this)
	{
		std::string asVal = std::to_string(this->GetValue());
		this->SendParamValueToServer(asVal);
	}
}

void InspUIImplInt::OnDestroy(wxWindowDestroyEvent& evt)
{
	this->destroyed = true;
}