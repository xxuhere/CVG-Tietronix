#include "InspUIImplEnum.h"
#include "InspBarParam.h"

BEGIN_EVENT_TABLE(InspUIImplEnum, wxCheckBox)
	EVT_COMBOBOX(-1, InspUIImplEnum::OnValueChanged)
	EVT_WINDOW_DESTROY(InspUIImplEnum::OnDestroy)
END_EVENT_TABLE()

wxArrayString InspUIImplEnum::CreateOptionsString(CVG::ParamSPtr p)
{
	wxArrayString ret;

	for(const std::string & str : p->GetPossible())
		ret.push_back(str);

	return ret;
}

InspUIImplEnum::InspUIImplEnum(InspBarParam* owner, CVGBridge* bridge)
	:	InspUIImplParam(owner, bridge),
		wxComboBox(
			owner, 
			-1, 
			"", 
			wxDefaultPosition, 
			wxDefaultSize, 
			CreateOptionsString(owner->param),
			wxCB_READONLY)
		
{
	this->UpdateDisplay();
}

void InspUIImplEnum::UpdateDisplay()
{
	std::string str;
	this->parent->param->GetValue(str);
	this->SetValue(str);
}

void InspUIImplEnum::DestroyUI()
{
	if(this->destroyed)
		return;

	this->Destroy();
}

wxWindow* InspUIImplEnum::GetWindow()
{
	return 
		this->destroyed ?
			nullptr:
			this;
}

void InspUIImplEnum::OnValueChanged(wxCommandEvent& evt)
{
	this->SendParamValueToServer(this->GetValue().ToStdString());
}

void InspUIImplEnum::OnDestroy(wxWindowDestroyEvent& evt)
{
	this->destroyed = true;
}