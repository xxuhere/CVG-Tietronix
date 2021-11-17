#pragma once

#include "App.h"
#include "DockedCVGPane.h"

class RootWindow;

/// <summary>
/// The pane containing the dashboard.
/// </summary>
class PaneDashboard : 
	public wxWindow,
	public DockedCVGPane
{
public:
	/// <summary>
	/// UI IDs.
	/// </summary>
	enum CmdIDs
	{
		/// <summary>
		/// The 'delete preset' button.
		/// </summary>
		Delete,

		/// <summary>
		/// The 'add preset' button.
		/// </summary>
		AddNew,

		/// <summary>
		/// The 'copy current preset' button.
		/// </summary>
		Copy
	};
private:

	/// <summary>
	/// The reference to the main application that owns this pane.
	/// </summary>
	RootWindow * rootWin;

	/// <summary>
	/// The pulldown list of preset selections.
	/// </summary>
	wxComboBox * presetPulldown;

	/// <summary>
	/// The 'add preset' button.
	/// </summary>
	wxButton* btnAddPreset;

	/// <summary>
	/// The 'delete preset' button.
	/// </summary>
	wxButton* btnDelPreset;

	/// <summary>
	/// The 'copy current preset' button.
	/// </summary>
	wxButton* btnCpyPreset;

public:
	PaneDashboard(wxWindow * win, int id, RootWindow * rootWin);


public: // DockedCVGPane OVERRIDE FUNCTIONS
	wxWindow * _CVGWindow() override;
	std::string Title() override;

	void _CVG_OnMessage(const std::string & msg) override;
	void _CVG_OnJSON(const json & js) override;
	void _CVG_EVT_OnConnect() override;
	void _CVG_EVT_OnDisconnect() override;

	void Canvas_OnMotion(wxMouseEvent& evt);
	void Canvas_OnLeftDown(wxMouseEvent& evt);
	void Canvas_OnLeftUp(wxMouseEvent& evt);
	void Canvas_OnPaint(wxPaintDC& evt);

	void _CVG_EVT_OnNewEquipment(CVG::BaseEqSPtr eq) override;
	void _CVG_EVT_OnRemEquipment(CVG::BaseEqSPtr eq) override;
	void _CVG_EVT_OnParamChange(CVG::BaseEqSPtr eq, CVG::ParamSPtr param) override;

	wxDECLARE_EVENT_TABLE();
};

