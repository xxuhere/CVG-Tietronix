#include "InspUIImplParam.h"
#include "InspBarParam.h"

InspUIImplParam::InspUIImplParam(InspBarParam * parent, CVGBridge* bridge)
{
	this->parent = parent;
	this->bridge = bridge;
}

InspUIImplParam::~InspUIImplParam()
{
	wxWindow * win = this->GetWindow();
	if(win != nullptr)
		this->DestroyUI();
}


void InspUIImplParam::SendParamValueToServer(const std::string & valAsString)
{
	this->bridge->CVGB_SetValue(
		this->parent->eqGUID,
		this->parent->param->GetID(),
		valAsString);
}

void InspUIImplParam::DestroyUI()
{
}

wxWindow* InspUIImplParam::GetWindow()
{
	return nullptr;
}