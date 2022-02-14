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

	// Bit flag representing the draggable edges of a dashboard element.
	// When describing a drag operation, at least one horizontal or one vertical
	// flag must be used. And both direction (i.e., left/right or top/bottom) cannot
	// be used at the same time.
	static const int RSZLEFTFLAG	= 1 << 1;
	static const int RSZRIGHTFLAG	= 1 << 2;
	static const int RSZTOPFLAG		= 1 << 3;
	static const int RSZBOTFLAG		= 1 << 4;

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

		/// <summary>
		/// The vertical scrollbar for the canvas.
		/// 
		/// May be unused if the dashboard is using a wxScrolledWindow.
		/// </summary>
		HorizCanvasScroll,

		/// <summary>
		/// The horizontal scrollbar for the canvas.
		/// 
		/// May be unused if the dashboard is using a wxScrolledWindow.
		/// </summary>
		VertCanvasScroll,

		/// <summary>
		/// The menu option to delete a dashboard element - where the menu is invoked
		/// by right clicking the element.
		/// </summary>
		DeleteRClick,

		/// <summary>
		/// Menu option to change an element's label to an explicitly defined value.
		/// </summary>
		RelabelRClick,

		/// <summary>
		/// Menu option to reset the element's label to its default value.
		/// </summary>
		ResetLabelRClick,

		PresetCombo,

		/// <summary>
		/// The mouse interaction choice - specifying which mouse drag operation is used.
		/// </summary>
		InteractionChoice,

		CheckboxDarkMode,

		/// <summary>
		/// The menu option to resize the dashboard grid - where the menu is invoked
		/// by right clicking the grid (where an element isn't present).
		/// </summary>
		Menu_ResizeDash,
	};

	/// <summary>
	/// The current state of interaction for the mouse.
	/// </summary>
	enum MouseInteractMode
	{
		/// <summary>
		/// GUI elements can be interacted with or moved (if non-GUI
		/// space can be accessed with the mouse).
		/// </summary>
		MoveOp,

		/// <summary>
		/// GUI elements can be moved around.
		/// </summary>
		Move,

		/// <summary>
		/// GUI elements can be resized.
		/// </summary>
		Resize,

		/// <summary>
		/// GUI elements can be interacted with, but not modified.
		/// </summary>
		Operate
	};

	/// <summary>
	/// Styles to draw the boundaries of individual tiles.
	/// </summary>
	enum class GridBoundsDrawMode
	{
		/// <summary>
		/// Boundary lines are not shown.
		/// </summary>
		Invisible,

		/// <summary>
		/// Heavy contrast lines.
		/// </summary>
		Heavy,

		/// <summary>
		/// An outline that more resembles the background than Heavy.
		/// </summary>
		Light,

		/// <summary>
		/// A dotted line pattern.
		/// </summary>
		Dotted
	};

	enum MouseDragMode
	{
		/// <summary>
		/// No mouse drag operation is occuring.
		/// </summary>
		None,

		/// <summary>
		/// The user had dragged an item from the inspector.
		/// </summary>
		PalleteInsert,

		/// <summary>
		/// The user is dragging elements to reposition them.
		/// </summary>
		DragElement,

		/// <summary>
		/// The user is dragging elements to resize them.
		/// </summary>
		ResizeElement
	};
private:

	/// <summary>
	/// The window containing the drawn canvas and tile UI elements.
	/// </summary>
	wxScrolledWindow * canvasWin = nullptr;

	// The container of canvasWin that holds its spot if it's 
	// detached (for fullscreen) so we know where to reattach it
	// when the canvas exits fullscreen.
	wxBoxSizer * canvasSizer = nullptr;

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

	// The instance of a grid being reified by the UI and Dash*Inst classes.
	DashboardGridInst* gridInst;

	/// <summary>
	/// How tile outlines are drawn in the UI.
	/// </summary>
	GridBoundsDrawMode drawOutlineMode = GridBoundsDrawMode::Heavy;

	/// <summary>
	/// Cached value for the target of right click menu operations.
	/// 
	/// It is set when a DashboardElement is right clicked, and should
	/// only be referenced in right click menu handlers.
	/// </summary>
	DashboardTile* rightClickedTile = nullptr;

	/// <summary>
	/// The node being moved around from a mouse drag.
	/// </summary>
	DashboardTile* draggedReposTile = nullptr;

	/// <summary>
	/// When performing a drag operation on an element, what was 
	/// the offset of where the element was clicked?
	/// </summary>
	wxPoint draggOffset;

	MouseDragMode dragMode = MouseDragMode::None;

	/// <summary>
	/// The choice pulldown for the interaction modes.
	/// </summary>
	wxChoice* choiceInteractions = nullptr;

	/// <summary>
	/// Checkbox to enable darkmode, where the canvas outside the
	/// grid is filled black.
	/// </summary>
	wxCheckBox* checkboxDark = nullptr;

	/// <summary>
	/// The editing mode, for how mouse interaction should be handled in the UI.
	/// </summary>
	MouseInteractMode interactionMode = MouseInteractMode::MoveOp;

	// The sides being dragged during an element resize.
	int resizeFlags = 0;
	// The point and size calculate for resizing. They're cached
	// as variable so they can be calculated once on mouse moves,
	// and shared between the OnPaint preview and the actual
	// resizing.
	wxPoint resizePoint;
	wxSize resizeSize;

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

	/// <summary>
	/// In certain editing/operating modes, the child windows of dashboard tiles aren't 
	/// shown because proxies are down in their place (to allow mouse editing to happen 
	/// where the child windows are).
	/// 
	/// When the 
	/// </summary>
	/// <returns>
	/// True if widgets should be shown. 
	/// Else, widget should be invisible.</returns>
	bool ShouldShowWidgetUIs()
	{ 
		return 
			this->interactionMode == MouseInteractMode::MoveOp || 
			this->interactionMode == MouseInteractMode::Operate;
	}

	/// <summary>
	/// Change the dashboard being viewed in the UI.
	/// </summary>
	/// <param name="index">
	/// The index of the dashboard to view. This should be a value between 0 and 
	/// DashdocCount().</param>
	/// <returns>
	/// True if the function switch to a new dashboard. Else, the index was
	/// already the one being viewed, or the index was invalid.
	/// </returns>
	bool SwitchToDashDoc(int index);

	/// <summary>
	/// Get the index of the current dashboard being viewed.
	/// </summary>
	/// <returns>The index of the current dashboard being viewed.</returns>
	int GetDashDocIndex();

	/// <summary>
	/// Get access to the current DashboardGrid data that the UI is showing.
	/// This will be pointer to the dashboard represented by this->GetDashDocIndex().
	/// </summary>
	/// <returns>The current DashboardGrid data that the UI is showing.</returns>
	DashboardGrid* GetGrid();

	// Drag and drop handlers when dropping new elements from 
	// and InspectorParam.
	void OnStartParamDrag(const std::string& eq, DashDragCont dc);
	void OnEndParamDrag(const std::string& eq, DashDragCont dc);
	void OnCancelParamDrag();
	void OnParamDrag(const std::string& eq, DashDragCont dc);

	/// <summary>
	/// Create an equipment param tile in the current dashboard.
	/// </summary>
	/// <returns>True if success.</returns>
	bool CreateParam(int cellX, int cellY, int cellWidth, int cellHeight, const std::string& eqGUID, CVG::ParamSPtr param);

	/// <summary>
	/// Create a (web)camera tile in the current dashboard.
	/// </summary>
	/// <returns>True if success.</returns>
	bool CreateCamera(int cellX, int cellY, int cellWidth, int cellHeight, const std::string& eqGUID, const CamChannel & camChan);

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
	void _CVG_EVT_OnRemapEquipmentPurpose(const std::string& purpose, const std::string& prevGUID, const std::string& newGUID ) override;

	void _CVG_Session_SavePre() override;
	void _CVG_Session_SavePost() override;
	void _CVG_Session_OpenPre(bool append) override;
	void _CVG_Session_OpenPost(bool append) override;
	void _CVG_Session_ClearPre() override;
	void _CVG_Session_ClearPost() override;

	// Event handlers for Dashboard document modifications
	void OnDashDoc_New(				DashboardGrid* newGrid);
	void OnDashDoc_Del(				DashboardGrid* deletedGrid);
	void OnDashDoc_NewElement(		DashboardGrid* grid,		DashboardTile* newTile);
	void OnDashDoc_RemElement(		DashboardGrid* grid,		DashboardTile* removedTile);
	void OnDashDoc_RelabelElement(	DashboardGrid* grid,		DashboardTile* removedTile);
	void OnDashDoc_ReposElement(	DashboardGrid* grid,		DashboardTile* modTile);
	void OnDashDoc_ResizeElement(	DashboardGrid* grid,		DashboardTile* modTile);
	void OnDashDoc_MovedElement(	DashboardGrid* grid,		DashboardTile* movedTile);
	void OnDashDoc_Renamed(			DashboardGrid* grid);
	void OnDashDoc_Resized(			DashboardGrid* grid);

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

	/// <summary>
	/// Changes the outline mode of how tile boundaries are drawn.
	/// The function also triggers a redraw of the canvas.
	/// </summary>
	/// <param name="outlineMode">The new outline draw mode.</param>
	void SetDrawDashboardOutline(GridBoundsDrawMode outlineMode);

	/// <summary>
	/// Enters and exits fullscreen of the dashboard.
	/// Should only be used on the main dashboard.
	/// </summary>
	/// <param name="fullscreen">
	/// If true the dashboard goes fullscreen. 
	/// Else, the dashboard restores from fullscreen.
	/// </param>
	void ToggleFullscreen(bool fullscreen);

	void OnMenuDeleteRightClicked(wxCommandEvent& evt);
	void OnMenuReLabelRightClicked(wxCommandEvent& evt);
	void OnMenuResetLabelRightClicked(wxCommandEvent& evt);
	void OnBtnNewDocument(wxCommandEvent& evt);
	void OnBtnDeleteCurDocument(wxCommandEvent& evt);
	void OnBtnCopyCurDocument(wxCommandEvent& evt);
	void OnComboPreset(wxCommandEvent& evt);
	void OnChoiceInteraction(wxCommandEvent& evt);
	void OnCheckboxDarkMode(wxCommandEvent& evt);
	void OnEnterPreset(wxCommandEvent& evt);
	void OnFocusPreset(wxFocusEvent& evt);

	void OnClose(wxCloseEvent& evt);

	void OnMenu_ResizeDashboardGrid(wxCommandEvent& evt);

	/// <summary>
	/// Called whenever ANY mouse dragging session is over. The function
	/// will clear any and all dragging state information - regardless of
	/// the actual specific drag operation.
	/// </summary>
	void _ClearMouseDragState();

	/// <summary>
	/// Detach the main dashboard canvas from the application.
	/// This should only be called for the main dashboard.
	/// 
	/// Used during fullscreen to deparent from app UI so it can
	/// be reparent to the fullscreen window.
	/// </summary>
	void DetachCanvas();

	/// <summary>
	/// Reattach the main dashboard canvas to the application.
	/// This should only be called for the main dashboard.
	/// 
	/// Called when restoring UI from fullscreen, to reparent the
	/// canvas from the fullscreen window back to the main app.
	/// </summary>
	void ReattachCanvas();

	wxDECLARE_EVENT_TABLE();

	// !TODO: destroy gridInst on close
};

