#pragma once

#include "App.h"
#include "DockedCVGPane.h"
#include "Inspector/InspBarParam.h"
#include <Params/Param.h>
#include <Equipment.h>
#include <map>
#include "Inspector/InspectorEquipment.h"


class RootWindow;

/// <summary>
/// The pane containing the inspector.
/// </summary>
class PaneInspector : 
	public wxWindow,
	public DockedCVGPane
{
private:
	/// <summary>
	/// The sizer for the highest level UI element.
	/// </summary>
	wxBoxSizer* mainSizer;

	/// <summary>
	/// Reference to the main application that owns this pane.
	/// </summary>
	RootWindow* rootWin;

	/// <summary>
	/// The scrollable region containing the listed inspector information.
	/// </summary>
	wxScrolledWindow* inspectorView;

	/// <summary>
	/// The sizer for inspectorView to layout all the inspectable data.
	/// </summary>
	wxBoxSizer* inspectorSizer;

	/// <summary>
	/// If true, the UI has already been flagged as dirty, along with
	/// a queued message to rebuild the UI.
	/// </summary>
	bool layoutDirty = false;

	/// <summary>
	/// The InspectorEquipment UI elements in the inspector view.
	/// </summary>
	std::map<std::string, InspectorEquipment*> equipmentUIGroups;

public:
	PaneInspector(wxWindow* kwin, int id, RootWindow* rootWin);

	PaneType GetPaneType() override;

	/// <summary>
	/// Destroy the current layout (including the sizer) and recreate one
	/// from scratch. 
	/// 
	/// While a simple layout can be done on a size change, a recreate should
	/// be done between moments when new equipments are added or removed from
	/// the inspector.
	/// </summary>
	void RecreateEquipmentLayout();

	inline wxScrolledWindow * InspectorScroll()
	{ return this->inspectorView;}

	/// <summary>
	/// Flags the inspector as dirty. A Layout is queued - but not performed instantly,
	/// to allow for multiple things to flag the layout as dirty without the overhead
	/// of continually refixing it.
	/// </summary>
	void FlagLayoutDirty();

	/// <summary>
	/// The message handler for a dirty layout.
	/// </summary>
	void OnLayoutDirty(wxCommandEvent& evt);

	/// <summary>
	/// Perform a layout on the scroll view.
	/// </summary>
	void LayoutEquipmentScrollView();

public: // DockedCVGPane OVERRIDE FUNCTIONS

	wxWindow * _CVGWindow() override;
	std::string Title() override;

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

