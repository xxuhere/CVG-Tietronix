#pragma once

#include "App.h"
#include "DockedCVGPane.h"
#include "Params/Param.h"
#include "DashboardGrid.h"

class RootWindow;

/// <summary>
/// The pane containing the dashboard.
/// </summary>
class PaneDashboard : 
	public wxWindow,
	public DockedCVGPane
{
public:

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
		Copy,

		HorizCanvasScroll,

		VertCanvasScroll,

		DeleteRClick,

		PresetCombo
	};

	enum class GridBoundsDrawMode
	{
		Invisible,
		Heavy,
		Light,
		Dotted
	};

	enum MouseDragMode
	{
		None,

		/// <summary>
		/// The user had dragged an item from the inspector.
		/// </summary>
		PalleteInsert,

		/// <summary>
		/// The user is dragging elements to reposition them.
		/// </summary>
		DragElement
	};
private:

	wxScrolledWindow * canvasWin = nullptr;

	/// <summary>
	/// The reference to the main application that owns this pane.
	/// </summary>
	RootWindow * rootWin = nullptr;

	/// <summary>
	/// The pulldown list of preset selections.
	/// </summary>
	wxComboBox * presetPulldown = nullptr;

	/// <summary>
	/// The 'add preset' button.
	/// </summary>
	wxButton* btnAddPreset = nullptr;

	/// <summary>
	/// The 'delete preset' button.
	/// </summary>
	wxButton* btnDelPreset = nullptr;

	/// <summary>
	/// The 'copy current preset' button.
	/// </summary>
	wxButton* btnCpyPreset = nullptr;

	wxScrollBar * vertCanvasScroll = nullptr;
	wxScrollBar * horizCanvaScroll = nullptr;


	// The instance of a grid being reified by the UI and Dash*Inst classes.
	DashboardGridInst* gridInst;

	GridBoundsDrawMode drawOutlineMode = GridBoundsDrawMode::Heavy;

	/// <summary>
	/// Cached value for the target of right click menu operations.
	/// 
	/// It is set when a DashboardElement is right clicked, and should
	/// only be referenced in right click menu handlers.
	/// </summary>
	DashboardElement* rightClickedEle = nullptr;

	/// <summary>
	/// The node being moved around from a mouse drag.
	/// </summary>
	DashboardElement* draggedReposEle = nullptr;

	/// <summary>
	/// When performing a drag operation on an element, what was 
	/// the offset of where the element was clicked?
	/// </summary>
	wxPoint draggOffset;

	MouseDragMode dragMode = MouseDragMode::None;

protected:
	void _ClearGrid();

public:
	PaneDashboard(wxWindow * win, int id, RootWindow * rootWin, DashboardGrid* defAttach);


public: // DockedCVGPane OVERRIDE FUNCTIONS
	wxWindow * _CVGWindow() override;
	std::string Title() override;
	PaneType GetPaneType() override;

	wxWindow* CanvasWin() const
	{ return this->canvasWin; }

	bool SwitchToDashDoc(int index);

	int GetDashDocIndex();

	DashboardGrid* GetGrid();

	// Drag and drop handlers when dropping new elements from 
	// and InspectorParam 
	void OnStartParamDrag(const std::string& eq, CVG::ParamSPtr param);
	void OnEndParamDrag(const std::string& eq, CVG::ParamSPtr param);
	void OnCancelParamDrag();
	void OnParamDrag(const std::string& eq, CVG::ParamSPtr param);

	bool CreateParam(int cellX, int cellY, int cellWidth, int cellHeight, const std::string& eqGUID, CVG::ParamSPtr param);

	// Main app messages
	void _CVG_OnMessage(const std::string & msg) override;
	void _CVG_OnJSON(const json & js) override;
	void _CVG_EVT_OnConnect() override;
	void _CVG_EVT_OnDisconnect() override;

	// App events for processed app messages
	void _CVG_EVT_OnNewEquipment(CVG::BaseEqSPtr eq) override;
	void _CVG_EVT_OnRemEquipment(CVG::BaseEqSPtr eq) override;
	void _CVG_EVT_OnParamChange(CVG::BaseEqSPtr eq, CVG::ParamSPtr param) override;
	void _CVG_Dash_NewBoard(DashboardGrid * addedGrid) override;
	void _CVG_Dash_DeleteBoard(DashboardGrid * remGrid) override;

	// Event handlers for Dashboard document modifications
	void OnDashDoc_New(DashboardGrid* newGrid);
	void OnDashDoc_Del(DashboardGrid* deletedGrid);
	void OnDashDoc_NewElement(DashboardGrid* grid, DashboardElement* newEle);
	void OnDashDoc_RemElement(DashboardGrid* grid, DashboardElement* removedEle);
	void OnDashDoc_ReposElement(DashboardGrid* grid, DashboardElement* modEle);
	void OnDashDoc_MovedElement(DashboardGrid* grid, DashboardElement* movedEle);
	void OnDashDoc_Renamed(DashboardGrid* grid);

	// Delegated event handlers for the dashboard canvas child
	void Canvas_OnMotion(wxMouseEvent& evt);
	void Canvas_OnLeftDown(wxMouseEvent& evt);
	void Canvas_OnLeftUp(wxMouseEvent& evt);
	void Canvas_OnPaint(wxPaintDC& evt);
	void Canvas_OnRightDown(wxMouseEvent& evt);
	void Canvas_OnScrollTop(wxScrollWinEvent& evt);
	void Canvas_OnScrollBot(wxScrollWinEvent& evt);
	void Canvas_OnScrollLineUp(wxScrollWinEvent& evt);
	void Canvas_OnScrollLineDown(wxScrollWinEvent& evt);
	void Canvas_OnScrollPageUp(wxScrollWinEvent& evt);
	void Canvas_OnScrollPageDown(wxScrollWinEvent& evt);
	void Canvas_OnScrollThumbTrack(wxScrollWinEvent& evt);
	void Canvas_OnScrollThumbRelease(wxScrollWinEvent& evt);

	void SetDrawDashboardOutline(GridBoundsDrawMode outlineMode);

	void OnTogglePreset(bool show);

	/// <summary>
	/// Should only be used on the main dashboard.
	/// </summary>
	/// <param name="fullscreen"></param>
	void ToggleFullscreen(bool fullscreen);

	void OnMenuDeleteRightClicked(wxCommandEvent& evt);
	void OnBtnNewDocument(wxCommandEvent& evt);
	void OnBtnDeleteCurDocument(wxCommandEvent& evt);
	void OnBtnCopyCurDocument(wxCommandEvent& evt);
	void OnComboPreset(wxCommandEvent& evt);
	void OnEnterPreset(wxCommandEvent& evt);
	void OnFocusPreset(wxFocusEvent& evt);

	void OnClose(wxCloseEvent& evt);

	void _ClearMouseDragState();

	wxDECLARE_EVENT_TABLE();

	// !TODO: destroy gridInst on close
};

