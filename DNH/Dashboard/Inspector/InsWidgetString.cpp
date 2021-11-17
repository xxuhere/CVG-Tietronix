#include "InsWidgetString.h"
#include "InspectorParam.h"

BEGIN_EVENT_TABLE(InsWidgetString, wxTextCtrl)
	EVT_WINDOW_DESTROY(InsWidgetString::OnDestroy)
	EVT_TEXT_ENTER(-1, InsWidgetString::OnTextEnter)
	EVT_SET_FOCUS(InsWidgetString::OnFocusChange)
END_EVENT_TABLE()

InsWidgetString::InsWidgetString(InspectorParam* owner, CVGBridge* bridge)
	:	InsWidgetParam(owner, bridge),
		wxTextCtrl(owner, -1, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER )
{
	this->UpdateDisplay();
}

void InsWidgetString::UpdateDisplay()
{
	CVG::ParamSPtr p = this->parent->param;

	std::string s;
	p->GetValue(s);
	this->SetValue(s);
}

void InsWidgetString::DestroyUI()
{
	if(this->destroyed)
		return;

	this->Destroy();
}

wxWindow* InsWidgetString::GetWindow()
{
	return 
		this->destroyed ?
		nullptr:
		this;
}

void InsWidgetString::OnTextEnter(wxCommandEvent& evt)
{
	this->SendParamValueToServer(this->GetValue().ToStdString());
}

void InsWidgetString::OnFocusChange(wxFocusEvent& evt)
{
	evt.Skip();

	// If we lost keyboard focus
	if(evt.GetWindow() == this)
		this->SendParamValueToServer(this->GetValue().ToStdString());
}

void InsWidgetString::OnDestroy(wxWindowDestroyEvent& evt)
{
	this->destroyed = true;
}