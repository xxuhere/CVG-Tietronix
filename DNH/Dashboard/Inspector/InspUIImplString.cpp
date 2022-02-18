#include "InspUIImplString.h"
#include "InspBarParam.h"

BEGIN_EVENT_TABLE(InspUIImplString, wxTextCtrl)
	EVT_WINDOW_DESTROY(InspUIImplString::OnDestroy)
	EVT_TEXT_ENTER(-1, InspUIImplString::OnTextEnter)
	EVT_SET_FOCUS(InspUIImplString::OnFocusChange)
END_EVENT_TABLE()

InspUIImplString::InspUIImplString(InspBarParam* owner, CVGBridge* bridge)
	:	InspUIImplParam(owner, bridge),
		wxTextCtrl(owner, -1, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER )
{
	this->UpdateDisplay();
}

void InspUIImplString::UpdateDisplay()
{
	CVG::ParamSPtr p = this->parent->param;

	std::string s;
	p->GetValue(s);
	this->SetValue(s);
}

void InspUIImplString::DestroyUI()
{
	if(this->destroyed)
		return;

	this->Destroy();
}

wxWindow* InspUIImplString::GetWindow()
{
	return 
		this->destroyed ?
		nullptr:
		this;
}

void InspUIImplString::OnTextEnter(wxCommandEvent& evt)
{
	this->SendParamValueToServer(this->GetValue().ToStdString());
}

void InspUIImplString::OnFocusChange(wxFocusEvent& evt)
{
	evt.Skip();

	// If we lost keyboard focus
	if(evt.GetWindow() == this)
		this->SendParamValueToServer(this->GetValue().ToStdString());
}

void InspUIImplString::OnDestroy(wxWindowDestroyEvent& evt)
{
	this->destroyed = true;
}