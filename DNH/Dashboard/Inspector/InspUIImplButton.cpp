#include "InspUIImplButton.h"
#include "InspBarParam.h"

BEGIN_EVENT_TABLE(InspUIImplButton, wxButton)
	EVT_BUTTON(-1, InspUIImplButton::OnButtonPress)
END_EVENT_TABLE()

InspUIImplButton::InspUIImplButton(InspBarParam* owner, CVGBridge* bridge)
	:	InspUIImplParam(owner, bridge),
		wxButton(owner, -1, "")
{
	std::string label = owner->param->GetLabel();
	this->SetLabel(label);
}

void InspUIImplButton::UpdateDisplay()
{
	// Do nothing
}

void InspUIImplButton::DestroyUI()
{
	this->Destroy();
}

wxWindow* InspUIImplButton::GetWindow()
{
	return this;
}

void InspUIImplButton::OnButtonPress(wxCommandEvent& evt)
{
	if(this->parent->param == nullptr)
		return;

	this->bridge->CVGB_Submit(
		this->parent->eqGUID,
		this->parent->param->GetID());
}