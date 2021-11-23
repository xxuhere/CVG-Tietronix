#include "DashElePulldown.h"
#include "../CVGBridge.h"

BEGIN_EVENT_TABLE(DashElePulldown, wxChoice)
	EVT_CHOICE(-1, DashElePulldown::OnChanged)
END_EVENT_TABLE();

DashElePulldown::DashElePulldown(
	wxWindow* parent, 
	DashboardElementInst* eleInst)
	:	IDashEle(eleInst),
		wxChoice(parent, -1)
{
	this->Reattach();
}

bool DashElePulldown::Reattach()
{
	CVG::ParamSPtr param = this->Param();
	this->Clear();
	if(param == nullptr)
	{
		this->Disable();
	}
	else
	{	
		this->Enable();

		std::vector<std::string> possibles = param->GetPossible();
		wxArrayString wxpossibs;
		for(const std::string& s : possibles)
			wxpossibs.Add(s);

		this->Append(wxpossibs);
		this->OnParamValueChanged();
	}
	return true;
}

std::string DashElePulldown::ImplName()
{
	return DASHELENAME_DEFENUM;
}

void DashElePulldown::Layout(const wxPoint& pixelPt, const wxSize& pixelSz, int cellSize)
{
	this->SetSize(wxRect(pixelPt, pixelSz));
}

void DashElePulldown::DestroyWindow()
{
	this->Destroy();
}

void DashElePulldown::OnParamValueChanged()
{
	CVG::ParamSPtr param = this->Param();
	if(param != nullptr)
	{ 
		std::string val;
		if(param->GetValue(val))
		{ 
			int selIdx = this->FindString(val, true);
			if(selIdx != -1)
				this->SetSelection(selIdx);
		}
	}
}

void DashElePulldown::OnChanged(wxCommandEvent& evt)
{
	if(this->Param() != nullptr)
	{ 
		this->Bridge()->CVGB_SetValue(
			this->eleInst->EqGUID(),
			this->Param()->GetID(), 
			this->GetStringSelection().ToStdString());
	}
}