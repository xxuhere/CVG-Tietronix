#include "RootWindow.h"
#include "PaneInspector.h"

wxDEFINE_EVENT(cvgEVTLayoutDirty, wxCommandEvent);

wxBEGIN_EVENT_TABLE(PaneInspector, wxWindow)
wx__DECLARE_EVT1(cvgEVTLayoutDirty, -1,      wxCommandEventHandler(PaneInspector::OnLayoutDirty))
wxEND_EVENT_TABLE()

PaneInspector::PaneInspector(wxWindow* win, int id, RootWindow* rootWin)
	: wxWindow(win, id, wxDefaultPosition, wxDefaultSize)
{
	this->rootWin = rootWin;

	this->mainSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(this->mainSizer);

	this->inspectorView = new wxScrolledWindow(this, -1, wxDefaultPosition, wxDefaultSize, wxALWAYS_SHOW_SB|wxVSCROLL);
	this->mainSizer->Add(this->inspectorView, 1, wxGROW);
	this->RecreateEquipmentLayout();

	this->inspectorView->SetScrollRate(20, 20);

	// If there's already equipment in the cache (if this was opened
	// after already being connected) then add existing stuff.
	for(auto it : this->rootWin->equipmentCache)
		this->_CVG_EVT_OnNewEquipment(it.second);
}

DockedCVGPane::PaneType PaneInspector::GetPaneType()
{
	return PaneType::Inspector;
}

void PaneInspector::RecreateEquipmentLayout()
{
	this->inspectorSizer = new wxBoxSizer(wxVERTICAL);
	this->inspectorView->SetSizer(this->inspectorSizer);

	// Order the panels so there's always a deterministic order
	// between the views in the client, as well as any other view.
	//
	// There is one exception, the representation of ourselves will always
	// be first.
	std::vector<InspectorEquipment*> order;

	for(auto it : this->equipmentUIGroups)
	{
		if(it.second->equipment->GUID() == this->rootWin->GUID())
		{
			// Add ourself if found, instead of queueing for sorting
			this->inspectorSizer->Add(it.second, 0, wxGROW|wxLEFT|wxRIGHT|wxTOP|wxBOTTOM, 2);
		}
		else
			order.push_back(it.second);
	}

	std::sort(
		order.begin(), 
		order.end(), 
		[](InspectorEquipment*a, InspectorEquipment*b)
		{
			return strcmp(a->equipment->GUID().c_str(), b->equipment->GUID().c_str()) > 0;
		});

	// Add the sorted items in.
	for(InspectorEquipment* ie: order)
		this->inspectorSizer->Add(ie, 0, wxGROW|wxLEFT|wxRIGHT|wxTOP|wxBOTTOM, 2);

	this->Layout();
	this->layoutDirty = false;
}

wxWindow * PaneInspector::_CVGWindow()
{ return this; }

std::string PaneInspector::Title()
{
	return "Equipment Inspector";
}

void PaneInspector::FlagLayoutDirty()
{
	// If it's dirty, there's already a handler queued.
	if(this->layoutDirty)
		return;

	this->layoutDirty = true;
	
	// Queue a handler for the dirty layout.
	wxCommandEvent event( cvgEVTLayoutDirty, -1);
	this->GetEventHandler()->AddPendingEvent( event );
}

void PaneInspector::OnLayoutDirty(wxCommandEvent& evt)
{
	this->layoutDirty = false;
	this->RecreateEquipmentLayout();
}

void PaneInspector::_CVG_OnMessage(const std::string & msg)
{
}

void PaneInspector::_CVG_OnJSON(const json & js)
{}

void PaneInspector::_CVG_EVT_OnConnect()
{
}

void PaneInspector::_CVG_EVT_OnDisconnect()
{
	for(auto it : this->equipmentUIGroups)
	{
		it.second->ClearReferences();
		it.second->Destroy();
	}
	this->equipmentUIGroups.clear();

	this->inspectorView->SetSizer(nullptr);
	this->mainSizer = nullptr;
}

void PaneInspector::_CVG_EVT_OnNewEquipment(CVG::BaseEqSPtr eq)
{
	InspectorEquipment * newIE = new InspectorEquipment(this, this->rootWin, eq);
	this->equipmentUIGroups[eq->GUID()] = newIE;
	if(eq->GUID() == this->rootWin->GUID())
		newIE->SetBGColorAsSelf();

	this->FlagLayoutDirty();
}

void PaneInspector::_CVG_EVT_OnRemEquipment(CVG::BaseEqSPtr eq)
{
	auto it = this->equipmentUIGroups.find(eq->GUID());
	if(it == this->equipmentUIGroups.end())
		return;

	it->second->Destroy();
	this->equipmentUIGroups.erase(it);
	this->FlagLayoutDirty();
}

void PaneInspector::LayoutEquipmentScrollView()
{
	this->inspectorView->Layout();
}

void PaneInspector::_CVG_EVT_OnParamChange(CVG::BaseEqSPtr eq, CVG::ParamSPtr param)
{
	auto itFindEq = this->equipmentUIGroups.find(eq->GUID());
	if(itFindEq == this->equipmentUIGroups.end())
		return;

	auto& paramUIMap = itFindEq->second->paramUIs;
	auto itFindParam = paramUIMap.find(param->GetID());
	if(itFindParam != paramUIMap.end())
		itFindParam->second->UpdateWidgetValue();
}

void PaneInspector::_CVG_Dash_NewBoard(DashboardGrid * addedGrid)
{}

void PaneInspector::_CVG_Dash_DeleteBoard(DashboardGrid * remGrid)
{}