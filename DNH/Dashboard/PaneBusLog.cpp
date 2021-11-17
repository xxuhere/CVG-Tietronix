#include "RootWindow.h"
#include "PaneBusLog.h"

wxBEGIN_EVENT_TABLE(PaneBusLog, wxWindow)
wxEND_EVENT_TABLE()

PaneBusLog::PaneBusLog(wxWindow * win, int id, RootWindow * rootWin)
	: wxWindow(win, id)
{
	this->rootWin = rootWin;

	this->logField = 
		new wxTextCtrl(
			this, 
			-1, 
			"", 
			wxDefaultPosition, 
			wxDefaultSize, 
			wxTE_MULTILINE|wxTE_READONLY|wxTE_BESTWRAP);

	wxBoxSizer * sizerMain = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(sizerMain);
	sizerMain->Add(this->logField, 1, wxEXPAND);
}

void PaneBusLog::AddEntry(const std::string & entry)
{
	int scrollRange = this->logField->GetScrollRange(wxVERTICAL);
	int scrollPos = this->logField->GetScrollPos(wxVERTICAL);

	this->logField->AppendText(entry + "\r\n\r\n");

	if(scrollPos == scrollRange)
	{ 
		this->logField->SetScrollPos(
			wxVERTICAL, 
			this->logField->GetScrollRange(wxVERTICAL));
	}
}

wxWindow * PaneBusLog::_CVGWindow()
{
	return this;
}

std::string PaneBusLog::Title()
{
	return "Bus Log";
}

void PaneBusLog::_CVG_OnMessage(const std::string & msg)
{
	this->AddEntry(msg);
}

void PaneBusLog::_CVG_OnJSON(const json & js)
{}

void PaneBusLog::_CVG_EVT_OnConnect()
{}

void PaneBusLog::_CVG_EVT_OnDisconnect()
{}

void PaneBusLog::_CVG_EVT_OnNewEquipment(CVG::BaseEqSPtr eq)
{}

void PaneBusLog::_CVG_EVT_OnRemEquipment(CVG::BaseEqSPtr eq)
{}

void PaneBusLog::_CVG_EVT_OnParamChange(CVG::BaseEqSPtr eq, CVG::ParamSPtr param)
{}