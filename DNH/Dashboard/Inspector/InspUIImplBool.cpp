#include "InspUIImplBool.h"
#include "InspBarParam.h"

BEGIN_EVENT_TABLE(InspUIImplBool, wxCheckBox)
	EVT_WINDOW_DESTROY(InspUIImplBool::OnDestroy)
	EVT_CHECKBOX(-1, InspUIImplBool::OnCheckbox)
END_EVENT_TABLE()

InspUIImplBool::InspUIImplBool(InspBarParam* owner, CVGBridge* bridge)
	:	InspUIImplParam(owner, bridge),
		wxCheckBox(owner, -1, "value")
{
	this->UpdateDisplay();
}

void InspUIImplBool::UpdateDisplay()
{
	CVG::ParamSPtr p = this->parent->param;

	bool b;
	p->GetValue(b);
	this->SetValue(b);
}

void InspUIImplBool::DestroyUI()
{
	if(this->destroyed)
		return;

	this->Destroy();
}

wxWindow* InspUIImplBool::GetWindow()
{
	return 
		this->destroyed ?
			nullptr:
			this;
}

void InspUIImplBool::OnCheckbox(wxCommandEvent& evt)
{
	this->SendParamValueToServer(this->GetValue() ? " true" : "");
}

void InspUIImplBool::OnDestroy(wxWindowDestroyEvent& evt)
{
	this->destroyed = true;
}