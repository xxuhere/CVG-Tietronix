#include "RootWindow.h"
#include "PaneDashboard.h"
#include "DashboardGridInst.h"
#include "DashboardElementInst.h"
#include "Dialogs/DlgGridSize.h"
#include "defines.h"
#include <wx/textdlg.h>
#include <algorithm>
#include <wx/glcanvas.h>
#include <GL/gl.h>

// The minimum width or height dimension of a dashboard element.
int MINCELLDIM = 2;

wxBEGIN_EVENT_TABLE(PaneDashboard, wxWindow)
	EVT_MENU( CmdIDs::DeleteRClick,			PaneDashboard::OnMenuDeleteRightClicked)
	EVT_MENU( CmdIDs::RelabelRClick,		PaneDashboard::OnMenuReLabelRightClicked)
	EVT_MENU( CmdIDs::ResetLabelRClick,		PaneDashboard::OnMenuResetLabelRightClicked)
	EVT_CLOSE(PaneDashboard::OnClose)
	EVT_BUTTON(CmdIDs::AddNew,				PaneDashboard::OnBtnNewDocument			)
	EVT_BUTTON(CmdIDs::Delete,				PaneDashboard::OnBtnDeleteCurDocument	)
	EVT_BUTTON(CmdIDs::Copy,				PaneDashboard::OnBtnCopyCurDocument		)
	EVT_CHOICE(CmdIDs::InteractionChoice,	PaneDashboard::OnChoiceInteraction		)
	EVT_CHECKBOX(CmdIDs::CheckboxDarkMode,	PaneDashboard::OnCheckboxDarkMode		)
	EVT_COMBOBOX(CmdIDs::PresetCombo,		PaneDashboard::OnComboPreset			)
	EVT_TEXT_ENTER(CmdIDs::PresetCombo,		PaneDashboard::OnEnterPreset			)
	EVT_MENU(CmdIDs::Menu_ResizeDash,		PaneDashboard::OnMenu_ResizeDashboardGrid)
wxEND_EVENT_TABLE()


PaneDashboard::PaneDashboard(wxWindow * win, int id, RootWindow * rootWin, DashboardGrid* defAttach)
	:	wxWindow(win, -1, wxDefaultPosition, wxDefaultSize)//,
		//grid(rootWin, GRIDCELLSIZE)
{
	this->rootWin = rootWin;

	wxPanel * topPanel = new wxPanel(this, -1);
	topPanel->SetMinSize(wxSize(30, 30));
	this->canvasWin = new wxScrolledWindow(this, -1);
	wxBoxSizer * mainSizer = new wxBoxSizer(wxVERTICAL);
	this->canvasSizer = new wxBoxSizer(wxVERTICAL);

	topPanel->SetBackgroundColour(wxColour(220, 220, 220));
	this->canvasWin->SetBackgroundColour(wxColour(240, 240, 240));

	// Sadly we can't double buffer the canvas. On Windows (untested
	// for Linux UIs) it doesn't play well embedded OpenGL children - 
	// those children OpenGL contexts will not draw at all.
	// (wleu 01/31/2022)
	//
	//this->canvasWin->SetDoubleBuffered(false);

	// Redirect the canvas' events to be handled by the main Dashboard class.
	this->canvasWin->Connect(wxEVT_MOTION,		(wxObjectEventFunction)&PaneDashboard::Canvas_OnMotion,		nullptr, this);
	this->canvasWin->Connect(wxEVT_LEFT_DOWN,	(wxObjectEventFunction)&PaneDashboard::Canvas_OnLeftDown,	nullptr, this);
	this->canvasWin->Connect(wxEVT_LEFT_UP,		(wxObjectEventFunction)&PaneDashboard::Canvas_OnLeftUp,		nullptr, this);
	this->canvasWin->Connect(wxEVT_PAINT,		(wxObjectEventFunction)&PaneDashboard::Canvas_OnPaint,		nullptr, this);
	this->canvasWin->Connect(wxEVT_RIGHT_DOWN,	(wxObjectEventFunction)&PaneDashboard::Canvas_OnRightDown,	nullptr, this);
	this->canvasWin->Connect(wxEVT_SCROLL_TOP,			(wxObjectEventFunction)&PaneDashboard::Canvas_OnScrollTop,			nullptr, this);
	this->canvasWin->Connect(wxEVT_SCROLL_BOTTOM,		(wxObjectEventFunction)&PaneDashboard::Canvas_OnScrollBot,			nullptr, this);
	this->canvasWin->Connect(wxEVT_SCROLL_LINEUP,		(wxObjectEventFunction)&PaneDashboard::Canvas_OnScrollLineUp,		nullptr, this);
	this->canvasWin->Connect(wxEVT_SCROLL_LINEDOWN,		(wxObjectEventFunction)&PaneDashboard::Canvas_OnScrollLineDown,		nullptr, this);
	this->canvasWin->Connect(wxEVT_SCROLL_PAGEUP,		(wxObjectEventFunction)&PaneDashboard::Canvas_OnScrollPageUp,		nullptr, this);
	this->canvasWin->Connect(wxEVT_SCROLL_PAGEDOWN,		(wxObjectEventFunction)&PaneDashboard::Canvas_OnScrollPageDown,		nullptr, this);
	this->canvasWin->Connect(wxEVT_SCROLL_THUMBTRACK,	(wxObjectEventFunction)&PaneDashboard::Canvas_OnScrollThumbTrack,	nullptr, this);
	this->canvasWin->Connect(wxEVT_SCROLL_THUMBRELEASE,	(wxObjectEventFunction)&PaneDashboard::Canvas_OnScrollThumbRelease,	nullptr, this);

	//this->vertCanvasScroll = new wxScrollBar(this, CmdIDs::VertCanvasScroll, wxDefaultPosition, wxDefaultSize, wxSB_VERTICAL);
	//this->horizCanvaScroll = new wxScrollBar(this, CmdIDs::HorizCanvasScroll, wxDefaultPosition, wxDefaultSize, wxSB_HORIZONTAL);
	
	wxFlexGridSizer * flexSizer = new wxFlexGridSizer(2);
	flexSizer->AddGrowableCol(0);
	flexSizer->AddGrowableRow(0);
	flexSizer->Add(this->canvasSizer, 1, wxGROW);
	this->canvasSizer->Add(this->canvasWin, 1, wxGROW);
	//flexSizer->Add(this->vertCanvasScroll, 0, wxGROW);
	//flexSizer->Add(this->horizCanvaScroll, 0, wxGROW);

	// Setup the layout of the high-level elements
	this->SetSizer(mainSizer);
	mainSizer->Add(topPanel, 0, wxGROW);
	mainSizer->Add(flexSizer, 1, wxGROW);

	// Setup the top bar
	wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
	topPanel->SetSizer(topSizer);
	this->presetPulldown = new wxComboBox(topPanel, CmdIDs::PresetCombo, "", wxDefaultPosition, wxDefaultSize, 0, nullptr, wxTE_PROCESS_ENTER);
	this->btnAddPreset = new wxButton(topPanel, CmdIDs::AddNew, "+");
	this->btnAddPreset->SetMaxSize(wxSize(50, 100));
	this->btnDelPreset = new wxButton(topPanel, CmdIDs::Delete, "X");
	this->btnDelPreset->SetMaxSize(wxSize(30, 100));
	this->btnCpyPreset = new wxButton(topPanel, CmdIDs::Copy, "Copy");
	this->btnCpyPreset->SetMaxSize(wxSize(50, 100));
	//
	// The order of the choices should match the indices of MouseInteractMode
	wxString intrChoices[] = {"MoveOp", "Move", "Resize", "Operate"};
	this->choiceInteractions = new wxChoice(topPanel, CmdIDs::InteractionChoice, wxDefaultPosition, wxDefaultSize, 4, intrChoices);
	this->choiceInteractions->Select(0);
	this->checkboxDark = new wxCheckBox(topPanel, CmdIDs::CheckboxDarkMode, "Dark Mode");
	//
	topSizer->Add(this->choiceInteractions);
	topSizer->AddSpacer(10);
	topSizer->Add(this->checkboxDark, 0, wxCENTER);
	topSizer->Add(0, 0, 1, wxGROW);
	topSizer->Add(this->btnAddPreset, 0, wxGROW);
	topSizer->Add(20, 0);
	topSizer->Add(this->btnCpyPreset, 0, wxGROW);
	topSizer->Add(this->presetPulldown, 0, wxGROW);
	topSizer->Add(this->btnDelPreset, 0, wxGROW);

	presetPulldown->SetMinSize(wxSize(150, 1));
	const size_t dashCt = this->rootWin->DashdocCount();
	for(size_t i = 0; i < dashCt; ++i)
	{
		DashboardGrid* g = this->rootWin->GetDashDoc(i);
		this->presetPulldown->Append(g->name);
	}
	int comboSel = this->rootWin->GetDashDocIndex(defAttach);
	this->presetPulldown->Select(comboSel);
	this->presetPulldown->Connect(wxEVT_SET_FOCUS, (wxObjectEventFunction)&PaneDashboard::OnFocusPreset, nullptr, this);

	if(defAttach != nullptr)
	{ 
		this->gridInst = new DashboardGridInst(this->canvasWin, rootWin, defAttach);
		this->gridInst->MatchEleInstLayouts();

		// Set the scrollable region to match the dashboard
		const int gridSize = defAttach->GridCellSize();
		this->canvasWin->SetVirtualSize(
			wxSize(
				defAttach->CellWidth() * gridSize,
				defAttach->CellHeight() * gridSize));

		this->canvasWin->SetScrollRate(1, 1);
	}

	wxGLCanvas * displayWin = new wxGLCanvas(this, wxID_ANY);
	static wxGLContextAttrs attrs;
	attrs.CoreProfile().Robust().EndList();
	wxGLContext * sharedContext = new wxGLContext(displayWin, nullptr, &attrs);
	sharedContext->SetCurrent(*displayWin);
	this->SetSize(0, 0, 300, 300);
	glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	displayWin->SwapBuffers();
}

wxWindow * PaneDashboard::_CVGWindow()
{
	return this;
}

std::string PaneDashboard::Title()
{
	return "Dashboard";
}

DockedCVGPane::PaneType PaneDashboard::GetPaneType()
{
	return PaneType::Dashboard;
}

void PaneDashboard::_ClearGrid()
{
	if(this->gridInst == nullptr)
		return;

	delete this->gridInst;
}

bool PaneDashboard::SwitchToDashDoc(int index)
{
	// Bound check
	if(index < 0 || index >= this->rootWin->DashdocCount())
		return false;

	DashboardGrid * targGrid = this->rootWin->GetDashDoc(index);
	if(this->gridInst != nullptr && targGrid == this->gridInst->Grid())
		return true;

	this->presetPulldown->SetValue(targGrid->name);
	this->_ClearGrid();

	// Set it as unused. In rare cases. The DashboardGridInst recreates
	// the new UI widget implementations. In rare cases this can trigger
	// a redraw which can crash if gridInst isn't tagged as null.
	this->gridInst = nullptr;

	this->gridInst = 
		new DashboardGridInst(
			this->canvasWin, 
			this->rootWin, 
			targGrid);

	this->gridInst->MatchEleInstLayouts();

	const int cellSz = this->gridInst->GridCellSize();
	
	this->canvasWin->SetScrollbars(
		1, 
		1, 
		cellSz * this->gridInst->CellWidth(),
		cellSz * this->gridInst->CellHeight(), 
		this->canvasWin->GetScrollPos(wxHORIZONTAL),
		this->canvasWin->GetScrollPos(wxVERTICAL));

	// UI widgets are created visible, but depending on the interaction
	// mode, they may need to be hidden.
	if(!this->ShouldShowWidgetUIs())
		this->gridInst->ToggleUIs(false);

	this->Refresh();
	return true;
}

DashboardGrid* PaneDashboard::GetGrid()
{
	if(this->gridInst == nullptr)
		return nullptr;

	return this->gridInst->Grid();
}


int PaneDashboard::GetDashDocIndex()
{
	if(this->gridInst == nullptr)
		return -1;

	return this->rootWin->GetDashDocIndex(this->gridInst->Grid());
}

void PaneDashboard::OnStartParamDrag(const std::string& eq, DashDragCont dc)
{
	this->dragMode = MouseDragMode::PalleteInsert;
	this->canvasWin->Refresh();
}

void PaneDashboard::OnEndParamDrag(const std::string& eq, DashDragCont dc)
{
	this->_ClearMouseDragState();
	wxPoint scroll = this->canvasWin->GetViewStart();

	wxPoint mousePos = wxGetMousePosition();
	wxPoint canvasScreenPos = this->canvasWin->GetScreenPosition();
	//
	int cellX = (mousePos.x - canvasScreenPos.x + scroll.x) / GRIDCELLSIZE;
	int cellY = (mousePos.y - canvasScreenPos.y + scroll.y) / GRIDCELLSIZE;

	// TODO: Reimplement
	if(this->gridInst->Grid()->InDocumentBounds(
		wxPoint(cellX, cellY), 
		wxSize(dc.cachedWidth, dc.cachedHeight)))
	{ 
		if(dc.type == DashDragCont::Type::Param)
			this->CreateParam(cellX, cellY, dc.cachedWidth, dc.cachedHeight, eq, dc.p);
		else if(dc.type == DashDragCont::Type::Cam)
			this->CreateCamera(cellX, cellY, dc.cachedWidth, dc.cachedHeight, eq, dc.camChan);
	}

	this->canvasWin->Refresh();
}

void PaneDashboard::OnCancelParamDrag()
{
	this->_ClearMouseDragState();
	this->canvasWin->Refresh();
}

void PaneDashboard::OnParamDrag(const std::string& eq, DashDragCont dc)
{
	// Redraw to show potential new drag spot.
	this->canvasWin->Refresh();
}

bool PaneDashboard::CreateParam(
	int cellX, 
	int cellY, 
	int cellWidth, 
	int cellHeight, 
	const std::string& eqGUID, 
	CVG::ParamSPtr param)
{
	DashboardElementInst* createdInst = 
		this->gridInst->AddDashboardElement(
			cellX, 
			cellY, 
			cellWidth,
			cellHeight, 
			eqGUID,
			param);

	if(createdInst == nullptr)
		return false;

	this->rootWin->BroadcastDashDoc_EleNew(
		this->gridInst->Grid(), 
		createdInst->RefEle());

	return true;
}

bool PaneDashboard::CreateCamera(
	int cellX,
	int cellY,
	int cellWidth,
	int cellHeight,
	const std::string& eqGUID,
	const CamChannel & camChan)
{
	TileCam* createdCam = 
		this->gridInst->AddDashboardCam(
			cellX, 
			cellY, 
			cellWidth,
			cellHeight, 
			eqGUID,
			camChan);
	
	if(createdCam == nullptr)
		return false;
	
	this->rootWin->BroadcastDashDoc_EleNew(
		this->gridInst->Grid(), 
		createdCam);

	return true;
}

void PaneDashboard::Canvas_OnMotion(wxMouseEvent& evt)
{
	// Remember the handler is for this->canvasWin, not PaneDashboard, it's just
	// being redirected for the PaneDashboard to handle.
	wxWindow * _this = this->canvasWin;

	switch(this->dragMode)
	{
	case MouseDragMode::PalleteInsert:
	case MouseDragMode::DragElement:
		_this->Refresh();
		break;

	case MouseDragMode::ResizeElement:
		{
			int cellSz = this->gridInst->GridCellSize();

			// The current cell pos and size.
			int cpx = this->draggedReposTile->CellPos().x;
			int cpy = this->draggedReposTile->CellPos().y;
			int cszx = this->draggedReposTile->CellSize().GetWidth();
			int cszy = this->draggedReposTile->CellSize().GetHeight();

			wxPoint scroll = this->canvasWin->GetViewStart();

			// The cell the mouse is at (while respecting the scrollbars)
			int mCellX = (evt.GetPosition().x + scroll.x) / cellSz;
			int mCellY = (evt.GetPosition().y + scroll.y) / cellSz;

			// Calculating dragging to resize. It's either a horizontal drag, a 
			// vertical drag, or both for a corner drag. The right and bottom are
			// a bit easier to calculate with clamping - because resizing left and
			// top require modifiying both the position and the size to make sure the
			// other end point doesn't move.
			if((this->resizeFlags & this->RSZTOPFLAG) != 0)
			{
				this->resizePoint.y = mCellY;
				if(cpy + cszy - this->resizePoint.y < MINCELLDIM)
					this->resizePoint.y = cpy + cszy - MINCELLDIM;

				this->resizeSize.y = cpy + cszy - this->resizePoint.y;
			}
			else if((this->resizeFlags & this->RSZBOTFLAG) != 0)
			{
				this->resizeSize.y = std::max(mCellY - cpy, MINCELLDIM);
			}
			if((this->resizeFlags & this->RSZLEFTFLAG) != 0)
			{
				this->resizePoint.x = mCellX;
				if(cpx + cszx - this->resizePoint.x < MINCELLDIM)
					this->resizePoint.x = cpx + cszx - MINCELLDIM;

				this->resizeSize.x = cpx + cszx - this->resizePoint.x;
			}
			else if((this->resizeFlags & this->RSZRIGHTFLAG) != 0)
			{
				this->resizeSize.x = std::max(mCellX - cpx, MINCELLDIM);
			}

			_this->Refresh();
		}
		break;
	}
}

void PaneDashboard::Canvas_OnRightDown(wxMouseEvent& evt)
{
	if(this->interactionMode == MouseInteractMode::Operate)
		return;

	wxScrolledWindow * _this = this->canvasWin;
	wxPoint clickPos = evt.GetPosition();
	clickPos += _this->GetViewStart(); // Account for scrollbar
	
	this->rightClickedTile = this->gridInst->Grid()->GetTileAtPixel(clickPos);
	if(this->rightClickedTile != nullptr)
	{
		wxMenu* eleRClickMenu = new wxMenu();
		eleRClickMenu->Append(CmdIDs::DeleteRClick, "Remove");
	
		if(this->rightClickedTile->Label() != this->rightClickedTile->DefaultLabel())
		{
			eleRClickMenu->Append(CmdIDs::ResetLabelRClick, "Reset Label");
		}
		
		eleRClickMenu->Append(CmdIDs::RelabelRClick, "Edit Label");
	
		this->PopupMenu(eleRClickMenu);
	}
	else
	{
		// If a dashboard element wasn't right clicked, open a right
		// click menu for the dashboard context.
		wxMenu * eleRClickMenu = new wxMenu();
		eleRClickMenu->Append(CmdIDs::Menu_ResizeDash, "Resize Grid");
		this->PopupMenu(eleRClickMenu);
	}
}

void PaneDashboard::Canvas_OnScrollTop(wxScrollWinEvent& evt)
{
	wxScrolledWindow * _this = this->canvasWin;
	
	// In case child window submits unhandled scrollbar message
	wxObject * obj = evt.GetEventObject();
	if(obj != _this)
	{
		evt.Skip(false);
		return;
	}
	
	int cellSize = this->gridInst->GridCellSize();
	
	_this->SetScrollbars(
		1, 
		1, 
		cellSize * this->gridInst->CellWidth(),
		cellSize * this->gridInst->CellHeight(), 
		_this->GetScrollPos(wxHORIZONTAL),
		0);
}

void PaneDashboard::Canvas_OnScrollBot(wxScrollWinEvent& evt)
{
	wxScrolledWindow * _this = this->canvasWin;
	
	// In case child window submits unhandled scrollbar message
	wxObject * obj = evt.GetEventObject();
	if(obj != _this)
	{
		evt.Skip(false);
		return;
	}
	
	int cellSize = this->gridInst->GridCellSize();
	
	_this->SetScrollbars(
		1, 
		1, 
		cellSize * this->gridInst->CellWidth(),
		cellSize * this->gridInst->CellHeight(), 
		_this->GetScrollPos(wxHORIZONTAL),
		_this->GetScrollRange(wxVERTICAL));
}

void PaneDashboard::Canvas_OnScrollLineUp(wxScrollWinEvent& evt)
{
	wxScrolledWindow * _this = this->canvasWin;
	
	// In case child window submits unhandled scrollbar message
	wxObject * obj = evt.GetEventObject();
	if(obj != _this)
	{
		evt.Skip(false);
		return;
	}
	
	int cellSize = this->gridInst->GridCellSize();
	
	_this->SetScrollbars(
		1, 
		1, 
		cellSize * this->gridInst->CellWidth(),
		cellSize * this->gridInst->CellHeight(), 
		_this->GetScrollPos(wxHORIZONTAL),
		_this->GetScrollPos(wxVERTICAL) - cellSize);
}

void PaneDashboard::Canvas_OnScrollLineDown(wxScrollWinEvent& evt)
{
	wxScrolledWindow * _this = this->canvasWin;
	
	// In case child window submits unhandled scrollbar message
	wxObject * obj = evt.GetEventObject();
	if(obj != _this)
	{
		evt.Skip(false);
		return;
	}
	
	int cellSize = this->gridInst->GridCellSize();
	
	_this->SetScrollbars(
		1, 
		1, 
		cellSize * this->gridInst->CellWidth(),
		cellSize * this->gridInst->CellHeight(), 
		_this->GetScrollPos(wxHORIZONTAL),
		_this->GetScrollPos(wxVERTICAL) + cellSize);
}

void PaneDashboard::Canvas_OnScrollPageUp(wxScrollWinEvent& evt)
{
	wxScrolledWindow * _this = this->canvasWin;
	
	// In case child window submits unhandled scrollbar message
	wxObject * obj = evt.GetEventObject();
	if(obj != _this)
	{
		evt.Skip(false);
		return;
	}
	
	int cellSize = this->gridInst->GridCellSize();
	wxSize clSize = _this->GetClientSize();
	
	_this->SetScrollbars(
		1, 
		1, 
		cellSize * this->gridInst->CellWidth(),
		cellSize * this->gridInst->CellHeight(), 
		_this->GetScrollPos(wxHORIZONTAL),
		_this->GetScrollPos(wxVERTICAL) - clSize.y);
}

void PaneDashboard::Canvas_OnScrollPageDown(wxScrollWinEvent& evt)
{
	wxScrolledWindow * _this = this->canvasWin;
	
	// In case child window submits unhandled scrollbar message
	wxObject * obj = evt.GetEventObject();
	if(obj != _this)
	{
		evt.Skip(false);
		return;
	}
	
	int cellSize = this->gridInst->GridCellSize();
	wxSize clSize = _this->GetClientSize();
	
	_this->SetScrollbars(
		1, 
		1, 
		cellSize * this->gridInst->CellWidth(),
		cellSize * this->gridInst->CellHeight(), 
		_this->GetScrollPos(wxHORIZONTAL),
		_this->GetScrollPos(wxVERTICAL) + clSize.y);
}

void PaneDashboard::Canvas_OnScrollThumbTrack(wxScrollWinEvent& evt)
{
	wxScrolledWindow * _this = this->canvasWin;
	
	// In case child window submits unhandled scrollbar message
	wxObject * obj = evt.GetEventObject();
	if(obj != _this)
	{
		evt.Skip(false);
		return;
	}
	
	int cellSize = this->gridInst->GridCellSize();
	
	if (evt.GetOrientation() == wxHORIZONTAL)
	{
		_this->SetScrollbars(
			1, 
			1, 
			cellSize * this->gridInst->CellWidth(),
			cellSize * this->gridInst->CellHeight(), 
			evt.GetPosition(),
			_this->GetScrollPos(wxVERTICAL));
	}
	else
	{
		_this->SetScrollbars(
			1, 
			1, 
			cellSize * this->gridInst->CellWidth(),
			cellSize * this->gridInst->CellHeight(), 
			_this->GetScrollPos(wxHORIZONTAL),
			evt.GetPosition());
	}
}

void PaneDashboard::Canvas_OnScrollThumbRelease(wxScrollWinEvent& evt)
{
	wxScrolledWindow * _this = this->canvasWin;
	
	// In case child window submits unhandled scrollbar message
	wxObject * obj = evt.GetEventObject();
	if(obj != _this)
	{
		evt.Skip(false);
		return;
	}
	
	int cellSize = this->gridInst->GridCellSize();
	
	if (evt.GetOrientation() == wxHORIZONTAL)
	{
		_this->SetScrollbars(
			1, 
			1, 
			cellSize * this->gridInst->CellWidth(),
			cellSize * this->gridInst->CellHeight(), 
			evt.GetPosition(),
			_this->GetScrollPos(wxVERTICAL));
	}
	else
	{
		_this->SetScrollbars(
			1, 
			1, 
			cellSize * this->gridInst->CellWidth(),
			cellSize * this->gridInst->CellHeight(), 
			_this->GetScrollPos(wxHORIZONTAL),
			evt.GetPosition());
	}
}

void PaneDashboard::Canvas_OnLeftDown(wxMouseEvent& evt)
{
	// Remember the handler is for this->canvasWin, not PaneDashboard, it's just
	// being redirected for the PaneDashboard to handle.
	wxScrolledWindow * _this = this->canvasWin;
	
	wxPoint clickPos = evt.GetPosition() + _this->GetViewStart();

	this->draggedReposTile = this->gridInst->Grid()->GetTileAtPixel(clickPos);
	if(this->draggedReposTile != nullptr)
	{
		wxPoint eleOrigin = 
			this->draggedReposTile->CellPos() * this->gridInst->GridCellSize();

		this->draggOffset = evt.GetPosition() - eleOrigin;

		if(
			this->interactionMode == MouseInteractMode::Move || 
			this->interactionMode == MouseInteractMode::MoveOp)
		{ 
			this->dragMode = MouseDragMode::DragElement;
			_this->CaptureMouse();
		}
		else if(this->interactionMode == MouseInteractMode::Resize)
		{
			const int dragAreaThk	= 10;
			int sideFlags = 0;

			// Find the combination of sides it's on. 
			// It will either be a 
			//	- horizontal side
			//  - vertical side
			//	- both a horizontal and vertical (a corner)
			//	- none (in the center).
			// If in the center, we'll consider it a move.
			//
			if(clickPos.x <= eleOrigin.x + dragAreaThk)
				sideFlags |= RSZLEFTFLAG;
			else if(clickPos.x >= eleOrigin.x + this->draggedReposTile->PixelSize().x - dragAreaThk)
				sideFlags |= RSZRIGHTFLAG;
			if(clickPos.y <= eleOrigin.y + dragAreaThk)
				sideFlags |= RSZTOPFLAG;
			if(clickPos.y >= eleOrigin.y + this->draggedReposTile->PixelSize().y - dragAreaThk)
				sideFlags |= RSZBOTFLAG;

			// Make sure a sane combination of one of the 8 allowed configurations
			// are allowed. If not, its either a state error or the center. Either
			// way, we'll fallback to a move if that happens.
			switch(sideFlags)
			{
			case RSZLEFTFLAG:
			case RSZRIGHTFLAG:
			case RSZTOPFLAG:
			case RSZBOTFLAG:
			case RSZLEFTFLAG|RSZTOPFLAG:
			case RSZRIGHTFLAG|RSZTOPFLAG:
			case RSZLEFTFLAG|RSZBOTFLAG:
			case RSZRIGHTFLAG|RSZBOTFLAG:
				this->dragMode = MouseDragMode::ResizeElement;
				this->resizeFlags = sideFlags;
				this->resizePoint = this->draggedReposTile->CellPos();
				this->resizeSize = this->draggedReposTile->CellSize();
				break;

			default:
				this->dragMode = MouseDragMode::DragElement;
				break;
			}
			_this->CaptureMouse();
		}
		else
		{
			this->draggedReposTile = nullptr;
		}
	}
}

void PaneDashboard::Canvas_OnPaint(wxPaintDC& evt)
{
	// This will only happen if the last grid is deleted. A redraw
	// may occur before a mandatory default dashboard is created.
	if(this->gridInst == nullptr)
		return;

	// Remember the handler is for this->canvasWin, not PaneDashboard, it's just
	// being redirected for the PaneDashboard to handle.
	wxScrolledWindow * _this = this->canvasWin;

	wxPaintDC dc(_this);
	//_this->PrepareDC(dc); We're going to take full control of handling the offsets.
	wxPoint scroll = _this->GetViewStart();


	// If darkmode is enabled, black out the background.
	bool usingDarkmode = this->checkboxDark->GetValue();
	if(usingDarkmode)
	{ 
		dc.SetBrush(*wxBLACK_BRUSH);
		dc.DrawRectangle(_this->GetClientRect());
	}

	// Draw the document outline.
	dc.SetPen(wxColour(0, 0, 0));
	if(usingDarkmode)
	{ 
		wxBrush bgBrush = wxBrush(this->GetBackgroundColour());
		dc.SetBrush(bgBrush);
	}
	else
	{ 
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
	}
	dc.DrawRectangle(
		-scroll, 
		wxSize(
			this->gridInst->CellWidth()	* this->gridInst->GridCellSize(), 
			this->gridInst->CellHeight() * this->gridInst->GridCellSize()));

	wxPen outlinePen;
	switch(this->drawOutlineMode)
	{
	case GridBoundsDrawMode::Heavy:
		outlinePen = wxPen(wxColour(128, 128, 128));
		break;

	case GridBoundsDrawMode::Light:
		outlinePen = wxPen(wxColour(200, 200, 200));
		break;

	case GridBoundsDrawMode::Dotted:
		outlinePen = wxPen(wxColour(128, 128, 128), 1, wxPENSTYLE_DOT);
		break;
	}

	for( const Tile* tile : *this->gridInst->Grid())
	{ 
		// If in the middle of a drag repositioning, draw visual
		// feedback to user.
		if(this->draggedReposTile == tile)
		{
			dc.SetPen(wxColour(0, 255, 0));
			dc.SetBrush(wxColour(50, 200, 50));
			dc.DrawRectangle(tile->PixelPos() - scroll, tile->PixelSize());
			dc.SetBrush(*wxTRANSPARENT_BRUSH);
		}
		else if(this->drawOutlineMode != GridBoundsDrawMode::Invisible)
		{ 
			dc.SetPen(outlinePen);
			dc.DrawRectangle(tile->PixelPos() - scroll, tile->PixelSize());
		}

		// Instead of having static text labels, we're going to
		// manually be in charge of drawing on the canvas.
		wxPoint textPos = tile->PixelPos();
		textPos.x += 10;
		textPos.y += 2;
		dc.DrawText(tile->Label(), textPos - scroll);
	}

	// If we shouldn't show widget UIs, draw their simplified
	// proxies instead.
	if(this->ShouldShowWidgetUIs() == false)
	{
		wxPoint prevOffset = scroll;
		for( auto it : *this->gridInst)
		{
			it.second->DrawImplPreview(dc, prevOffset);
		}
	}

	const int gridCellSz = this->gridInst->GridCellSize();
	// Draw the insertion preview when dragging something 
	// from the inspector
	if(this->dragMode == MouseDragMode::PalleteInsert)
	{ 
		dc.SetPen(*wxTRANSPARENT_PEN);

		int defWidth = this->rootWin->globalInspectorDrag.cachedWidth;
		int defHeight = this->rootWin->globalInspectorDrag.cachedHeight;

		wxPoint cell = (wxGetMousePosition() - _this->GetScreenPosition() + scroll) / GRIDCELLSIZE;

		if (this->gridInst->Grid()->InDocumentBounds(
			cell,
			wxSize(defWidth, defHeight)))
		{
			dc.SetBrush(wxBrush(wxColour(200, 255, 0)));
		}
		else
			dc.SetBrush(wxBrush(wxColour(255, 200, 200)));


		wxPoint pixelPos = cell * this->gridInst->GridCellSize();

		dc.DrawRectangle(
			pixelPos - scroll, 
			wxSize(
				this->gridInst->GridCellSize() * defWidth, 
				this->gridInst->GridCellSize() * defHeight));
	}
	else if(this->dragMode == MouseDragMode::DragElement)
	{
		// If we're dragging the mouse to reposition an element
		assert(this->draggedReposTile != nullptr);

		dc.SetPen(*wxTRANSPARENT_PEN);

		wxPoint cell = wxGetMousePosition() - _this->GetScreenPosition() - this->draggOffset;
		cell = (cell/gridCellSz);
		wxPoint pixelPos = cell * gridCellSz;
		
		dc.SetBrush(wxBrush(wxColour(200, 255, 200)));
		dc.DrawRectangle(
			pixelPos - scroll, 
			this->draggedReposTile->CellSize() * gridCellSz);
	}
	else if(this->dragMode == MouseDragMode::ResizeElement)
	{
		// If we're dragging the mouse to resize an element.
		dc.SetPen(*wxTRANSPARENT_PEN);
		dc.SetBrush(wxBrush(wxColour(200, 255, 200)));
		dc.DrawRectangle(
			this->resizePoint * gridCellSz - scroll, 
			this->resizeSize * gridCellSz);
	}
}

void PaneDashboard::Canvas_OnLeftUp(wxMouseEvent& evt)
{
	// Remember the handler is for this->canvasWin, not PaneDashboard, it's just
	// being redirected for the PaneDashboard to handle.
	wxWindow * _this = this->canvasWin;

	if(_this->HasCapture())
		_this->ReleaseMouse();

	switch(this->dragMode)
	{
	// PalleteInsert Handled in OnEndParamDrag()
	//case MouseDragMode::PalleteInsert:
	//	break;

	case MouseDragMode::DragElement:
		// Is an DashboardElement was being dragged, see if we can move it.
		if(this->draggedReposTile != nullptr)
		{
			wxPoint offsetPt = evt.GetPosition() - this->draggOffset;
			wxPoint cellMove = offsetPt / this->gridInst->GridCellSize();

			bool moved = 
				this->gridInst->MoveCell(
					this->draggedReposTile,
					cellMove,
					this->draggedReposTile->CellSize());

			if(moved)
			{ 
				rootWin->BroadcastDashDoc_EleMoved(
					this->gridInst->Grid(), 
					this->draggedReposTile);
			}
			else
			{
				// Immediately redraw to get rid of drag preview on the canvas.
				this->Refresh();
			}
		}
		break;

	case MouseDragMode::ResizeElement:
		if(this->draggedReposTile != nullptr)
		{
			// All the size calculation has already been done in Canvas_OnMotion().
			if(this->draggedReposTile->SetDimensions(this->resizePoint, this->resizeSize))
			{
				rootWin->BroadcastDashDoc_EleResize(
					this->gridInst->Grid(), 
					this->draggedReposTile);
			}
			else
			{
				this->Refresh();
			}
		}
		break;
	}
	this->_ClearMouseDragState();
}


void PaneDashboard::_CVG_OnMessage(const std::string & msg)
{}

void PaneDashboard::_CVG_OnJSON(const json & js)
{
	if(js["apity"] == "equipment")
	{
		// If we process equipment and things are remapped, the underlying
		// document is kept in sync, but we still have to refresh the UI.
		this->gridInst->RefreshInstances();
	}
}

void PaneDashboard::_CVG_EVT_OnConnect()
{}

void PaneDashboard::_CVG_EVT_OnDisconnect()
{}

void PaneDashboard::_CVG_EVT_OnNewEquipment(CVG::BaseEqSPtr eq)
{}

void PaneDashboard::_CVG_EVT_OnRemEquipment(CVG::BaseEqSPtr eq)
{}

void PaneDashboard::_CVG_EVT_OnRemapEquipmentPurpose(const std::string& purpose, const std::string& prevGUID, const std::string& newGUID )
{
	this->gridInst->RemapInstance(prevGUID, newGUID);
}

void PaneDashboard::_CVG_EVT_OnParamChange(CVG::BaseEqSPtr eq, CVG::ParamSPtr param)
{
	this->gridInst->UpdateParamValue(eq->GUID(), param->GetID());
}

void PaneDashboard::SetDrawDashboardOutline(GridBoundsDrawMode outlineMode)
{
	if(this->drawOutlineMode == outlineMode)
		return;

	this->drawOutlineMode = outlineMode;
	this->canvasWin->Refresh();
}

void PaneDashboard::_ClearMouseDragState()
{
	this->dragMode = MouseDragMode::None;
	this->draggedReposTile = nullptr;
	this->resizeFlags = 0;
}

void PaneDashboard::_CVG_Dash_NewBoard(DashboardGrid * addedGrid)
{
	// Nothing happens on reaction to a new dashboard.
}

void PaneDashboard::_CVG_Dash_DeleteBoard(DashboardGrid * remGrid)
{
	// TODO: If not a main dashboard and it's a doc we're viewing, auto close.
	// If the main dashboard, switch to the closed other doc
}

void PaneDashboard::_CVG_Session_SavePre()
{
}

void PaneDashboard::_CVG_Session_SavePost()
{
}

void PaneDashboard::_CVG_Session_OpenPre(bool append)
{
	if(append == true)
		this->presetPulldown->Clear();
}

void PaneDashboard::_CVG_Session_OpenPost(bool append)
{
	if(this->gridInst != nullptr)
		this->gridInst->RefreshInstances();

	const int cellSz = this->gridInst->GridCellSize();
	
	this->canvasWin->SetScrollbars(
		1, 
		1, 
		cellSz * this->gridInst->CellWidth(),
		cellSz * this->gridInst->CellHeight(), 
		this->canvasWin->GetScrollPos(wxHORIZONTAL),
		this->canvasWin->GetScrollPos(wxVERTICAL));
}

void PaneDashboard::_CVG_Session_ClearPre()
{
	this->presetPulldown->Clear();

	if(this->gridInst != nullptr)
	{ 
		DashboardGridInst* oldInst = this->gridInst;
		// Null it out since the DashboardGridInst will destroy the UI elements
		// which risks redrawing the canvas - which will crash if holding a 
		// deleted and non-null gridInst.
		this->gridInst = nullptr;
		delete oldInst;
	}
	this->Refresh();
}

void PaneDashboard::_CVG_Session_ClearPost()
{
}

void PaneDashboard::OnDashDoc_New(DashboardGrid* newGrid)
{
	this->presetPulldown->Append(newGrid->name);
}

void PaneDashboard::OnDashDoc_Del(DashboardGrid* deletedGrid)
{
	int idx = this->rootWin->GetDashDocIndex(deletedGrid);
	if(idx != -1)
		this->presetPulldown->Delete(idx);

	if(this != this->rootWin->MainDashboardPane())
		this->rootWin->CloseRegistered(this);

	// -2: 
	//	-1 to be below the count and 
	//	-1 to to account for the deleted item.
	int newIdx = std::max<int>(idx, this->rootWin->DashdocCount() - 2);

	// The only reason this should fail is if the last DashboardGrid
	// was destroyed. We'll force it to recreate a new dashboard if 
	// this is the main dashboard panel, but we have to wait until after
	// the notifications are done. Until then we'll flag the gridInst
	// as empty so things can know not to use it.
	if(!this->SwitchToDashDoc(newIdx))
		this->gridInst = nullptr;
}

void PaneDashboard::OnDashDoc_NewElement(DashboardGrid* grid, Tile* newTile)
{
	if(this->gridInst->Grid() != grid)
		return;

	if(newTile->GetType() == Tile::Type::Param)
	{
		TileParam* ele = (TileParam*)newTile;
		this->gridInst->Implement(ele);
	}
	else if(newTile->GetType() == Tile::Type::Cam)
	{
		TileCam* cam = (TileCam*)newTile;
		this->gridInst->Implement(cam);
	}

	this->Refresh();
}

void PaneDashboard::OnDashDoc_RemElement(DashboardGrid* grid, Tile* removedTile)
{
	if(this->gridInst->Grid() != grid)
		return;

	// No need to delete the actual dashboard element. This function 
	// is being called because something else is already deleting it.
	// Just remove the attached UI.
	this->gridInst->RemoveTile(removedTile, false);
	this->Refresh();
}

void PaneDashboard::OnDashDoc_RelabelElement(DashboardGrid* grid, Tile* removedEle)
{
	if(this->gridInst->Grid() != grid)
		return;

	this->Refresh();
}

void PaneDashboard::OnDashDoc_ReposElement(DashboardGrid* grid, Tile* modTile)
{
	if(this->gridInst->Grid() != grid)
		return;

	if(modTile->GetType() == Tile::Type::Param)
		this->gridInst->MatchEleInstLayout((TileParam*)modTile);
	else if(modTile->GetType() == Tile::Type::Cam)
		this->gridInst->MatchEleInstLayout((TileCam*)modTile);

	this->canvasWin->Refresh();
}

void PaneDashboard::OnDashDoc_ResizeElement(DashboardGrid* grid, Tile* modTile)
{
	// Arguably we could get rid of this and just have 
	// OnDashDoc_ReposElement() since they do the same thing.
	if(this->gridInst->Grid() != grid)
		return;

	if(modTile->GetType() == Tile::Type::Param)
		this->gridInst->MatchEleInstLayout((TileParam*)modTile);
	else if(modTile->GetType() == Tile::Type::Cam)
		this->gridInst->MatchEleInstLayout((TileCam*)modTile);

	this->canvasWin->Refresh();
}

void PaneDashboard::OnDashDoc_MovedElement(DashboardGrid* grid, Tile* movedTile)
{
	if(this->gridInst->Grid() != grid)
		return;

	if(movedTile->GetType() == Tile::Type::Param)
		this->gridInst->MatchEleInstLayout((TileParam*)movedTile);
	else if(movedTile->GetType() == Tile::Type::Cam)
		this->gridInst->MatchEleInstLayout((TileCam*)movedTile);

	this->canvasWin->Refresh();
}

void PaneDashboard::OnDashDoc_Renamed(DashboardGrid* grid)
{
	// On a rename, change the pulldown.

	// If it's the active item, change the text.
	if(this->gridInst->Grid() == grid)
		this->presetPulldown->SetValue(grid->name);

	// Change it in the names pulldown.
	int idx = this->rootWin->GetDashDocIndex(grid);
	this->presetPulldown->Delete(idx);
	this->presetPulldown->Insert(grid->name, idx);
}

void PaneDashboard::OnDashDoc_Resized(DashboardGrid* grid)
{
	if(this->gridInst->Grid() == grid)
		this->Refresh();
}

void PaneDashboard::OnMenuDeleteRightClicked(wxCommandEvent& evt)
{
	assert(this->rightClickedTile != nullptr);

	if(this->gridInst->Grid()->Remove(this->rightClickedTile) == false)
	{
		assert(!"Could not successfully remove node.");
		return;
	}
	
	if (this->gridInst->RemoveTile(this->rightClickedTile, false))
	{
		this->rootWin->BroadcastDashDoc_EleRem(
			this->gridInst->Grid(), 
			this->rightClickedTile);
	}
}

void PaneDashboard::OnMenuReLabelRightClicked(wxCommandEvent& evt)
{
	assert(this->rightClickedTile != nullptr);

	wxTextEntryDialog dlgQueryLabel(this, "Label", "Change Label", this->rightClickedTile->Label());
	int dlgRet = dlgQueryLabel.ShowModal();
	if(dlgRet != wxID_OK)
		return;
	
	std::string newLabel = dlgQueryLabel.GetValue().ToStdString();

	if(this->rightClickedTile->GetType() == Tile::Type::Param)
	{
		TileParam* ele = (TileParam*)this->rightClickedTile;
		if(newLabel.empty())
		{
			newLabel = ele->Param()->GetLabel();
			if(newLabel.empty())
				newLabel = ele->Param()->GetID();
		}
	}

	this->rightClickedTile->SetLabel(newLabel);
	this->rootWin->BroadcastDashDoc_EleRelabled(this->gridInst->Grid(), this->rightClickedTile);
}

void PaneDashboard::OnMenuResetLabelRightClicked(wxCommandEvent& evt)
{
	assert(this->rightClickedTile != nullptr);
	this->rightClickedTile->SetLabel("");
	this->rootWin->BroadcastDashDoc_EleRelabled(this->gridInst->Grid(), this->rightClickedTile);
}

void PaneDashboard::OnBtnNewDocument(wxCommandEvent& evt)
{
	DashboardGrid* newGrid = this->rootWin->CreateNewDashDoc("NewLayout");
	
	int docIdx = this->rootWin->GetDashDocIndex(newGrid);
	// Sanity check
	if(docIdx == -1)
		return;

	this->SwitchToDashDoc(docIdx);
}

void PaneDashboard::OnBtnDeleteCurDocument(wxCommandEvent& evt)
{
	assert(this->gridInst != nullptr);
	assert(this->gridInst->Grid() != nullptr);

	this->rootWin->DeleteDashDoc(this->gridInst->Grid());
	// We'll flush the UI when we handle the dashdoc delete notification.
	// (See PaneDashboard::OnDashDoc_Del())
}

void PaneDashboard::OnBtnCopyCurDocument(wxCommandEvent& evt)
{
	assert(this->gridInst != nullptr);
	assert(this->gridInst->Grid() != nullptr);
	DashboardGrid* newGrid = this->rootWin->DuplicateDashDoc(this->gridInst->Grid());

	int docIdx = this->rootWin->GetDashDocIndex(newGrid);
	// Sanity check
	if(docIdx == -1)
		return;

	this->SwitchToDashDoc(docIdx);
}

void PaneDashboard::OnComboPreset(wxCommandEvent& evt)
{
	int sel = this->presetPulldown->GetSelection();
	this->SwitchToDashDoc(sel);
}

void PaneDashboard::OnChoiceInteraction(wxCommandEvent& evt)
{
	this->interactionMode = 
		(MouseInteractMode)this->choiceInteractions->GetCurrentSelection();

	this->gridInst->ToggleUIs(this->ShouldShowWidgetUIs());

	this->Refresh();
}

void PaneDashboard::OnCheckboxDarkMode(wxCommandEvent& evt)
{
	this->Refresh();
}

void PaneDashboard::OnEnterPreset(wxCommandEvent& evt)
{
	std::string presetVal = this->presetPulldown->GetValue().ToStdString();

	if(presetVal.empty())
		this->presetPulldown->SetValue(this->gridInst->Grid()->name);
	else
	{ 
		this->gridInst->Grid()->name = presetVal;
		this->rootWin->BroadcastDashDoc_Renamed(this->gridInst->Grid());
	}
}

void PaneDashboard::OnFocusPreset(wxFocusEvent& evt)
{
	evt.Skip();

	if(evt.GetWindow() == this)
	{ 
		std::string presetVal = this->presetPulldown->GetValue().ToStdString();

		if(presetVal.empty())
			this->presetPulldown->SetValue(this->gridInst->Grid()->name);
		else
		{ 
			this->gridInst->Grid()->name = presetVal;
			this->rootWin->BroadcastDashDoc_Renamed(this->gridInst->Grid());
		}
	}
}

void PaneDashboard::DetachCanvas()
{
	this->canvasSizer->Remove(0);
}

void PaneDashboard::ReattachCanvas()
{
	// Reparent from the fullscreen dashboard back to this dashboard.
	// See FullscreenDash::DetachCanvas() - which is called right before
	// this ReattachCanvas() - for more information.
	this->canvasWin->Reparent(this);
	this->canvasSizer->Add(this->canvasWin, 1, wxGROW);
}

void PaneDashboard::OnClose(wxCloseEvent& evt)
{
	evt.Skip();
	this->_ClearGrid();
}

void PaneDashboard::OnMenu_ResizeDashboardGrid(wxCommandEvent& evt)
{
	wxSize gridSz = 
		wxSize(
			this->gridInst->Grid()->CellWidth(),
			this->gridInst->Grid()->CellHeight());

	DlgGridSize dlgGridSz(this, gridSz);
	int modalRet = dlgGridSz.ShowModal();
	if(modalRet == wxID_OK)
	{
		if(this->gridInst->Grid()->SetCellSize(dlgGridSz.size))
			this->rootWin->BroadcastDashDoc_Resized(this->gridInst->Grid());

		const int cellSz = this->gridInst->GridCellSize();

		this->canvasWin->SetScrollbars(
			1, 
			1, 
			cellSz * this->gridInst->CellWidth(),
			cellSz * this->gridInst->CellHeight(), 
			this->canvasWin->GetScrollPos(wxHORIZONTAL),
			this->canvasWin->GetScrollPos(wxVERTICAL));
	}
}