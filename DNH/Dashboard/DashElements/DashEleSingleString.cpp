#include "DashEleSingleString.h"
#include "../CVGBridge.h"

BEGIN_EVENT_TABLE(DashEleSingleString, wxTextCtrl)
	EVT_TEXT_ENTER(-1, DashEleSingleString::OnInputEnter)
	EVT_SET_FOCUS( DashEleSingleString::OnInputFocus)
END_EVENT_TABLE()

DashEleSingleString::DashEleSingleString(wxWindow* parent, DashboardElementInst* eleInst)
	:	IDashEle(eleInst),
		wxTextCtrl(parent, -1, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER)
{
	this->Reattach();
}

bool DashEleSingleString::Reattach()
{
	if(this->Param() == nullptr )
	{
		this->Disable();
		this->SetValue("");
	}
	else
	{
		this->Enable();
		this->OnParamValueChanged();
	}
	return true;
}

std::string DashEleSingleString::ImplName()
{
	return DASHELENAME_DEFSTRING;
}

void DashEleSingleString::Layout(const wxPoint& pixelPt, const wxSize& pixelSz, int cellSize)
{
	this->SetSize(wxRect(pixelPt, pixelSz));
}

void DashEleSingleString::DestroyWindow()
{
	this->Destroy();
}

void DashEleSingleString::OnParamValueChanged()
{
	std::string strVal;
	this->Param()->GetValue(strVal);
	this->SetValue(strVal);
}

void DashEleSingleString::OnInputEnter(wxCommandEvent& evt)
{
	
	CVG::ParamSPtr param = this->Param();
	if(param == nullptr)
		return;

	this->Bridge()->CVGB_SetValue(
		this->eleInst->EqGUID(),
		param->GetID(),
		this->GetValue().ToStdString());
}

void DashEleSingleString::OnInputFocus(wxFocusEvent& evt)
{
	evt.Skip();

	// Only call if loosing keyboard focus (not gaining)
	if(evt.GetWindow() == this)
	{ 
		CVG::ParamSPtr param = this->Param();
		
		if(param == nullptr)
			return;

		this->Bridge()->CVGB_SetValue(
			this->eleInst->EqGUID(),
			param->GetID(),
			this->GetValue().ToStdString());
	}
}