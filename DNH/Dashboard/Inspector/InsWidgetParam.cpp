#include "InsWidgetParam.h"
#include "InspectorParam.h"

InsWidgetParam::InsWidgetParam(InspectorParam * parent, CVGBridge* bridge)
{
	this->parent = parent;
	this->bridge = bridge;
}

InsWidgetParam::~InsWidgetParam()
{
	wxWindow * win = this->GetWindow();
	if(win != nullptr)
		this->DestroyUI();
}


void InsWidgetParam::SendParamValueToServer(const std::string & valAsString)
{
	this->bridge->CVGB_SetValue(
		this->parent->eqGUID,
		this->parent->param->GetID(),
		valAsString);
}

void InsWidgetParam::DestroyUI()
{
}

wxWindow* InsWidgetParam::GetWindow()
{
	return nullptr;
}