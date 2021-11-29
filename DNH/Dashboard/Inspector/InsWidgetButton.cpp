#include "InsWidgetButton.h"
#include "InspectorParam.h"

BEGIN_EVENT_TABLE(InsWidgetButton, wxButton)
	EVT_BUTTON(-1, InsWidgetButton::OnButtonPress)
END_EVENT_TABLE()

InsWidgetButton::InsWidgetButton(InspectorParam* owner, CVGBridge* bridge)
	:	InsWidgetParam(owner, bridge),
		wxButton(owner, -1, "")
{
	std::string label = owner->param->GetLabel();
	this->SetLabel(label);
}

void InsWidgetButton::UpdateDisplay()
{
	// Do nothing
}

void InsWidgetButton::DestroyUI()
{
	this->Destroy();
}

wxWindow* InsWidgetButton::GetWindow()
{
	return this;
}

void InsWidgetButton::OnButtonPress(wxCommandEvent& evt)
{
	if(this->parent->param == nullptr)
		return;

	this->bridge->CVGB_Submit(
		this->parent->eqGUID,
		this->parent->param->GetID());
}