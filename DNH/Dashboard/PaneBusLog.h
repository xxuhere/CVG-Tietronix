#pragma once
#include "App.h"
#include "DockedCVGPane.h"

class RootWindow;

/// <summary>
/// The pane displaying a log of incomming messages.
/// </summary>
class PaneBusLog : 
	public wxWindow,
	public DockedCVGPane
{
	/// <summary>
	/// Reference to the main application.
	/// </summary>
	RootWindow * rootWin;

	/// <summary>
	/// The text log.
	/// </summary>
	wxTextCtrl * logField;

public:

	PaneBusLog(wxWindow * win, int id, RootWindow * rootWin);

	/// <summary>
	/// Add an entry to the displayed log.
	/// </summary>
	/// <param name="entry">The text payload to log.</param>
	void AddEntry(const std::string & entry);

public: // DockedCVGPane OVERRIDE FUNCTIONS
	wxWindow * _CVGWindow() override;
	std::string Title() override;
	PaneType GetPaneType() override;

	void _CVG_OnMessage(const std::string & msg) override;
	void _CVG_OnJSON(const json & js) override;
	void _CVG_EVT_OnConnect() override;
	void _CVG_EVT_OnDisconnect() override;

	void _CVG_EVT_OnNewEquipment(CVG::BaseEqSPtr eq) override;
	void _CVG_EVT_OnRemEquipment(CVG::BaseEqSPtr eq) override;
	void _CVG_EVT_OnParamChange(CVG::BaseEqSPtr eq, CVG::ParamSPtr param) override;
	void _CVG_Dash_NewBoard(DashboardGrid * addedGrid) override;
	void _CVG_Dash_DeleteBoard(DashboardGrid * remGrid) override;

	wxDECLARE_EVENT_TABLE();
};

