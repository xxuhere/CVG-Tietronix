#include "RootWindow.h"
#include "PaneDashboard.h"

wxBEGIN_EVENT_TABLE(PaneDashboard, wxWindow)
wxEND_EVENT_TABLE()


PaneDashboard::PaneDashboard(wxWindow * win, int id, RootWindow * rootWin)
	: wxWindow(win, -1, wxDefaultPosition, wxDefaultSize)
{
	this->rootWin = rootWin;

	wxPanel * topPanel = new wxPanel(this, -1);
	topPanel->SetMinSize(wxSize(30, 30));
	wxWindow * canvasWin = new wxWindow(this, -1);
	wxBoxSizer * mainSizer = new wxBoxSizer(wxVERTICAL);

	topPanel->SetBackgroundColour(wxColour(220, 220, 220));
	canvasWin->SetBackgroundColour(wxColour(240, 240, 240));

	// Redirect the canvas' events to be handled by the main Dashboard class.
	canvasWin->Connect(wxEVT_MOTION,	(wxObjectEventFunction)&PaneDashboard::Canvas_OnMotion,		nullptr, this);
	canvasWin->Connect(wxEVT_LEFT_DOWN, (wxObjectEventFunction)&PaneDashboard::Canvas_OnLeftDown,	nullptr, this);
	canvasWin->Connect(wxEVT_LEFT_UP,	(wxObjectEventFunction)&PaneDashboard::Canvas_OnLeftUp,		nullptr, this);
	canvasWin->Connect(wxEVT_PAINT,		(wxObjectEventFunction)&PaneDashboard::Canvas_OnPaint,		nullptr, this);

	// Setup the layout of the high-level elements
	this->SetSizer(mainSizer);
	mainSizer->Add(topPanel, 0, wxGROW);
	mainSizer->Add(canvasWin, 1, wxGROW);

	// Setup the top bar
	wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
	topPanel->SetSizer(topSizer);
	this->presetPulldown = new wxComboBox(topPanel, -1, "Default");
	this->btnAddPreset = new wxButton(topPanel, CmdIDs::AddNew, "+");
	this->btnAddPreset->SetMaxSize(wxSize(50, 100));
	this->btnDelPreset = new wxButton(topPanel, CmdIDs::Delete, "X");
	this->btnDelPreset->SetMaxSize(wxSize(30, 100));
	this->btnCpyPreset = new wxButton(topPanel, CmdIDs::Copy, "Copy");
	this->btnCpyPreset->SetMaxSize(wxSize(50, 100));
	//
	topSizer->Add(0, 0, 1, wxGROW);
	topSizer->Add(this->btnAddPreset, 0, wxGROW);
	topSizer->Add(20, 0);
	topSizer->Add(this->btnCpyPreset, 0, wxGROW);
	topSizer->Add(this->presetPulldown, 0, wxGROW);
	topSizer->Add(this->btnDelPreset, 0, wxGROW);
}

wxWindow * PaneDashboard::_CVGWindow()
{
	return this;
}

std::string PaneDashboard::Title()
{
	return "Dashboard";
}

void PaneDashboard::Canvas_OnMotion(wxMouseEvent& evt)
{
}

void PaneDashboard::Canvas_OnLeftDown(wxMouseEvent& evt)
{
}

void PaneDashboard::Canvas_OnPaint(wxPaintDC& evt)
{
}

void PaneDashboard::Canvas_OnLeftUp(wxMouseEvent& evt)
{
}


void PaneDashboard::_CVG_OnMessage(const std::string & msg)
{
}

void PaneDashboard::_CVG_OnJSON(const json & js)
{
}

void PaneDashboard::_CVG_EVT_OnConnect()
{
}

void PaneDashboard::_CVG_EVT_OnDisconnect()
{
}

void PaneDashboard::_CVG_EVT_OnNewEquipment(CVG::BaseEqSPtr eq)
{
}

void PaneDashboard::_CVG_EVT_OnRemEquipment(CVG::BaseEqSPtr eq)
{
}

void PaneDashboard::_CVG_EVT_OnParamChange(CVG::BaseEqSPtr eq, CVG::ParamSPtr param)
{
}