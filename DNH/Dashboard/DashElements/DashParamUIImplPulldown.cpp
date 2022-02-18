#include "DashParamUIImplPulldown.h"
#include "../CVGBridge.h"

BEGIN_EVENT_TABLE(DashParamUIImplPulldown, wxChoice)
	EVT_CHOICE(-1, DashParamUIImplPulldown::OnChanged)
END_EVENT_TABLE();

DashParamUIImplPulldown::DashParamUIImplPulldown(
	wxWindow* parent, 
	DashboardElementInst* eleInst)
	:	DashParamUIImpl(eleInst),
		wxChoice(parent, -1)
{
	this->Reattach();
}

bool DashParamUIImplPulldown::Reattach()
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

std::string DashParamUIImplPulldown::ImplName()
{
	return DASHELENAME_DEFENUM;
}

void DashParamUIImplPulldown::Layout(const wxPoint& pixelPt, const wxSize& pixelSz, int cellSize)
{
	this->SetSize(wxRect(pixelPt, pixelSz));
}

void DashParamUIImplPulldown::DestroyWindow()
{
	this->Destroy();
}

void DashParamUIImplPulldown::OnParamValueChanged()
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

void DashParamUIImplPulldown::DrawPreview(wxPaintDC& dc, const wxPoint& offset)
{
	dc.SetPen(*wxCYAN_PEN);
	dc.SetBrush(*wxWHITE_BRUSH);

	wxRect r = this->GetRect();
	dc.DrawRectangle(r);
}

void DashParamUIImplPulldown::Toggle(bool show)
{
	this->Show(show);
}

void DashParamUIImplPulldown::OnChanged(wxCommandEvent& evt)
{
	if(this->Param() != nullptr)
	{ 
		this->Bridge()->CVGB_SetValue(
			this->eleInst->EqGUID(),
			this->Param()->GetID(), 
			this->GetStringSelection().ToStdString());
	}
}