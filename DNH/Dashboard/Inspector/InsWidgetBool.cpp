#include "InsWidgetBool.h"
#include "InspectorParam.h"

BEGIN_EVENT_TABLE(InsWidgetBool, wxCheckBox)
	EVT_WINDOW_DESTROY(InsWidgetBool::OnDestroy)
	EVT_CHECKBOX(-1, InsWidgetBool::OnCheckbox)
END_EVENT_TABLE()

InsWidgetBool::InsWidgetBool(InspectorParam* owner, CVGBridge* bridge)
	:	InsWidgetParam(owner, bridge),
		wxCheckBox(owner, -1, "value")
{
	this->UpdateDisplay();
}

void InsWidgetBool::UpdateDisplay()
{
	CVG::ParamSPtr p = this->parent->param;

	bool b;
	p->GetValue(b);
	this->SetValue(b);
}

void InsWidgetBool::DestroyUI()
{
	if(this->destroyed)
		return;

	this->Destroy();
}

wxWindow* InsWidgetBool::GetWindow()
{
	return 
		this->destroyed ?
			nullptr:
			this;
}

void InsWidgetBool::OnCheckbox(wxCommandEvent& evt)
{
	this->SendParamValueToServer(this->GetValue() ? " true" : "");
}

void InsWidgetBool::OnDestroy(wxWindowDestroyEvent& evt)
{
	this->destroyed = true;
}