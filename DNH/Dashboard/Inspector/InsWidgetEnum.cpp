#include "InsWidgetEnum.h"
#include "InspectorParam.h"

BEGIN_EVENT_TABLE(InsWidgetEnum, wxCheckBox)
	EVT_COMBOBOX(-1, InsWidgetEnum::OnValueChanged)
	EVT_WINDOW_DESTROY(InsWidgetEnum::OnDestroy)
END_EVENT_TABLE()

wxArrayString InsWidgetEnum::CreateOptionsString(CVG::ParamSPtr p)
{
	wxArrayString ret;

	for(const std::string & str : p->GetPossible())
		ret.push_back(str);

	return ret;
}

InsWidgetEnum::InsWidgetEnum(InspectorParam* owner, CVGBridge* bridge)
	:	InsWidgetParam(owner, bridge),
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

void InsWidgetEnum::UpdateDisplay()
{
	std::string str;
	this->parent->param->GetValue(str);
	this->SetValue(str);
}

void InsWidgetEnum::DestroyUI()
{
	if(this->destroyed)
		return;

	this->Destroy();
}

wxWindow* InsWidgetEnum::GetWindow()
{
	return 
		this->destroyed ?
			nullptr:
			this;
}

void InsWidgetEnum::OnValueChanged(wxCommandEvent& evt)
{
	this->SendParamValueToServer(this->GetValue().ToStdString());
}

void InsWidgetEnum::OnDestroy(wxWindowDestroyEvent& evt)
{
	this->destroyed = true;
}