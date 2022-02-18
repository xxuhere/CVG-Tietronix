#include "InspectorEquipment.h"
#include "../PaneInspector.h"
#include "../CVGBridge.h"
#include "InspBarParam.h"
#include "InspBarVideoStream.h"

BEGIN_EVENT_TABLE(InspectorEquipment, wxWindow)
	EVT_BUTTON(CmdIDs::ExpandCompress, InspectorEquipment::OnButtonExpandCompress)
END_EVENT_TABLE()

InspectorEquipment::InspectorEquipment(
	PaneInspector * owner, 
	CVGBridge* bridge, 
	CVG::BaseEqSPtr eq)
	:	wxWindow(owner->InspectorScroll(), -1, wxDefaultPosition, wxDefaultSize)
{
	this->equipment = eq;
	this->owner = owner;

	// Create outer window
	this->outerSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(this->outerSizer);
	this->SetBGColorAsOther();

	// Add titlebar
	wxBoxSizer* titleSizer = new wxBoxSizer(wxHORIZONTAL);
	this->titleText = new wxStaticText(this, -1, eq->Name(), wxDefaultPosition, wxDefaultSize);
	this->outerSizer->Add(titleSizer, 0, wxEXPAND|wxTOP|wxBOTTOM|wxLEFT|wxRIGHT, 5);
	titleSizer->Add(this->titleText, 1, wxEXPAND);
	//
	this->btnExpandCompress = new wxButton(this, CmdIDs::ExpandCompress, "[]");
	this->btnExpandCompress->SetMaxSize(wxSize(20, 100));
	titleSizer->Add(this->btnExpandCompress, 0, wxEXPAND);

	// Add param body
	this->paramWinContainer = new wxWindow(this, -1, wxDefaultPosition, wxDefaultSize);
	this->outerSizer->Add(this->paramWinContainer, 0, wxEXPAND|wxTOP|wxBOTTOM|wxLEFT|wxRIGHT, 5);
	this->paramSizer = new wxBoxSizer(wxVERTICAL);
	this->paramWinContainer->SetSizer(this->paramSizer);

	// Add params to body
	for(CVG::ParamSPtr params : *eq)
	{
		InspBarParam * ip = 
			new InspBarParam(
				this->paramWinContainer, 
				owner,
				bridge,
				eq->GUID(), 
				params);

		this->paramUIs[params->GetID()] = ip;
		this->paramSizer->Add(ip, 0, wxGROW|wxBOTTOM, 5);
	}

	std::vector<CamChannel> camChans = 
		CamChannel::ExtractChannels(eq->ClientData(), eq->Hostname());

	for(const CamChannel & cc : camChans)
	{
		InspBarVideoStream * iewc = 
			new InspBarVideoStream(
				this->paramWinContainer,
				eq->GUID(),
				bridge,
				cc);

		iewc->SetSize(-1, 32);
		this->paramCamChans.push_back(iewc);
		this->paramSizer->Add(iewc, 0, wxGROW|wxBOTTOM, 5);
	}

}

void InspectorEquipment::ClearReferences()
{
	this->equipment.reset();

	for(auto it : this->paramUIs)
		it.second->ClearReferences();
}

void InspectorEquipment::SetBGColorAsSelf()
{
	this->SetBackgroundColour(wxColour(200, 220, 200));
}

void InspectorEquipment::SetBGColorAsOther()
{
	this->SetBackgroundColour(wxColour(200, 200, 220));
}

void InspectorEquipment::OnButtonExpandCompress(wxCommandEvent& evt)
{
	this->expanded = !this->expanded;
	if(this->expanded)
	{
		this->btnExpandCompress->SetLabel("[]");
		this->paramWinContainer->Show(true);
	}
	else
	{
		this->btnExpandCompress->SetLabel("_");
		this->paramWinContainer->Show(false);
	}
	this->owner->LayoutEquipmentScrollView();
}