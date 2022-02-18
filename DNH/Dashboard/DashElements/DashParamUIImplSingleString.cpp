#include "DashParamUIImplSingleString.h"
#include "../CVGBridge.h"

BEGIN_EVENT_TABLE(DashParamUIImplSingleString, wxTextCtrl)
	EVT_TEXT_ENTER(-1, DashParamUIImplSingleString::OnInputEnter)
	EVT_SET_FOCUS( DashParamUIImplSingleString::OnInputFocus)
END_EVENT_TABLE()

DashParamUIImplSingleString::DashParamUIImplSingleString(wxWindow* parent, DashboardElementInst* eleInst)
	:	DashParamUIImpl(eleInst),
		wxTextCtrl(parent, -1, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER)
{
	this->Reattach();
}

bool DashParamUIImplSingleString::Reattach()
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

std::string DashParamUIImplSingleString::ImplName()
{
	return DASHELENAME_DEFSTRING;
}

void DashParamUIImplSingleString::Layout(const wxPoint& pixelPt, const wxSize& pixelSz, int cellSize)
{
	this->SetSize(wxRect(pixelPt, pixelSz));
}

void DashParamUIImplSingleString::DestroyWindow()
{
	this->Destroy();
}

void DashParamUIImplSingleString::OnParamValueChanged()
{
	std::string strVal;
	this->Param()->GetValue(strVal);
	this->SetValue(strVal);
}

void DashParamUIImplSingleString::DrawPreview(wxPaintDC& dc, const wxPoint& offset)
{
	dc.SetPen(*wxCYAN_PEN);
	dc.SetBrush(*wxWHITE_BRUSH);

	wxRect r = this->GetRect();
	dc.DrawRectangle(r);
}

void DashParamUIImplSingleString::Toggle(bool show)
{
	this->Show(show);
}

void DashParamUIImplSingleString::OnInputEnter(wxCommandEvent& evt)
{
	
	CVG::ParamSPtr param = this->Param();
	if(param == nullptr)
		return;

	this->Bridge()->CVGB_SetValue(
		this->eleInst->EqGUID(),
		param->GetID(),
		this->GetValue().ToStdString());
}

void DashParamUIImplSingleString::OnInputFocus(wxFocusEvent& evt)
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