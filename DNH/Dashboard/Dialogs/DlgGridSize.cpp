#include "DlgGridSize.h"

BEGIN_EVENT_TABLE(DlgGridSize, wxDialog)
	EVT_SPINCTRL((int)DlgGridSize::IDs::SpinX, DlgGridSize::OnSpinX)
	EVT_SPINCTRL((int)DlgGridSize::IDs::SpinY, DlgGridSize::OnSpinY)
END_EVENT_TABLE()

DlgGridSize::DlgGridSize(wxWindow* parent, const wxSize& startingSz)
	: wxDialog(parent, -1, "Change Dashboard Size", wxDefaultPosition, wxDefaultSize)
{
	this->size = startingSz;
	
	wxFlexGridSizer* gridSizer = new wxFlexGridSizer(3);
	
	this->spinX = new wxSpinCtrl(this, (int)IDs::SpinX, std::to_string(startingSz.GetX()), wxDefaultPosition, wxDefaultSize, 16384L, 10, 100, startingSz.GetX());
	this->spinY = new wxSpinCtrl(this, (int)IDs::SpinY, std::to_string(startingSz.GetY()), wxDefaultPosition, wxDefaultSize, 16384L, 10, 100, startingSz.GetY());
	
	gridSizer->AddGrowableCol(2);
	gridSizer->Add(new wxStaticText(this, -1, "Width"), 0, wxALIGN_RIGHT);
	gridSizer->AddSpacer(20);
	gridSizer->Add(this->spinX, 1, wxGROW);
	gridSizer->Add(new wxStaticText(this, -1, "Height"), 0, wxALIGN_RIGHT);
	gridSizer->AddSpacer(20);
	gridSizer->Add(this->spinY, 1, wxGROW);
	
	wxBoxSizer * mainSizer = new wxBoxSizer(wxVERTICAL);
	mainSizer->Add(gridSizer, 1, wxGROW);
	mainSizer->Add(this->CreateStdDialogButtonSizer(wxOK|wxCANCEL));
	
	this->SetSizer(mainSizer);

	this->Fit();
}

void DlgGridSize::OnSpinX(wxSpinEvent& evt)
{
	this->size.x = this->spinX->GetValue();
}

void DlgGridSize::OnSpinY(wxSpinEvent& evt)
{
	this->size.y = this->spinY->GetValue();
}