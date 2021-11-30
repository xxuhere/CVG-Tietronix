#include "RootWindow.h"
#include "PaneDashboard.h"
#include "DashboardGridInst.h"
#include "DashboardElementInst.h"
#include "Dialogs/DlgGridSize.h"
#include "defines.h"
#include <algorithm>

wxBEGIN_EVENT_TABLE(PaneDashboard, wxWindow)
	EVT_MENU( CmdIDs::DeleteRClick,		PaneDashboard::OnMenuDeleteRightClicked)
	EVT_CLOSE(PaneDashboard::OnClose)
	EVT_BUTTON(CmdIDs::AddNew,			PaneDashboard::OnBtnNewDocument			)
	EVT_BUTTON(CmdIDs::Delete,			PaneDashboard::OnBtnDeleteCurDocument	)
	EVT_BUTTON(CmdIDs::Copy,			PaneDashboard::OnBtnCopyCurDocument		)
	EVT_COMBOBOX(CmdIDs::PresetCombo,	PaneDashboard::OnComboPreset			)
	EVT_TEXT_ENTER(CmdIDs::PresetCombo,	PaneDashboard::OnEnterPreset			)
	EVT_MENU(CmdIDs::Menu_ResizeDash,	PaneDashboard::OnMenu_ResizeDashboardGrid)
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
	this->canvasWin->SetDoubleBuffered(true);

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

void PaneDashboard::OnStartParamDrag(const std::string& eq, CVG::ParamSPtr param)
{
	this->dragMode = MouseDragMode::PalleteInsert;
	this->canvasWin->Refresh();
}

void PaneDashboard::OnEndParamDrag(const std::string& eq, CVG::ParamSPtr param)
{
	this->_ClearMouseDragState();
	wxPoint scroll = this->canvasWin->GetViewStart();

	wxPoint mousePos = wxGetMousePosition();
	wxPoint canvasScreenPos = this->canvasWin->GetScreenPosition();
	//
	int cellX = (mousePos.x - canvasScreenPos.x + scroll.x) / GRIDCELLSIZE;
	int cellY = (mousePos.y - canvasScreenPos.y + scroll.y) / GRIDCELLSIZE;

	int cellWidth = 20; // Arbitrarily set
	int cellHeight = 5;

	// TODO: Reimplement
	if(this->gridInst->Grid()->InDocumentBounds(
		wxPoint(cellX, cellY), 
		wxSize(cellWidth, cellHeight)))
	{ 
		this->CreateParam(cellX, cellY, cellWidth, cellHeight, eq, param);
	}

	this->canvasWin->Refresh();
}

void PaneDashboard::OnCancelParamDrag()
{
	this->_ClearMouseDragState();
	this->canvasWin->Refresh();
}

void PaneDashboard::OnParamDrag(const std::string& eq, CVG::ParamSPtr param)
{
	// Redraw to show potential new drag spot.
	this->canvasWin->Refresh();
}

bool PaneDashboard::CreateParam(int cellX, int cellY, int cellWidth, int cellHeight, const std::string& eqGUID, CVG::ParamSPtr param)
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

void PaneDashboard::Canvas_OnMotion(wxMouseEvent& evt)
{
	// Remember the handler is for this->canvasWin, not PaneDashboard, it's just
	// being redirected for the PaneDashboard to handle.
	wxWindow * _this = this->canvasWin;

	switch(this->dragMode)
	{
	case MouseDragMode::PalleteInsert:
		_this->Refresh();
		break;

	case MouseDragMode::DragElement:
		_this->Refresh();
		break;
	}
}

void PaneDashboard::Canvas_OnRightDown(wxMouseEvent& evt)
{
	wxScrolledWindow * _this = this->canvasWin;
	wxPoint clickPos = evt.GetPosition();
	clickPos += _this->GetViewStart(); // Account for scrollbar

	this->rightClickedEle = this->gridInst->Grid()->GetDashboardAtPixel(clickPos);
	if(this->rightClickedEle != nullptr)
	{
		wxMenu* eleRClickMenu = new wxMenu();
		eleRClickMenu->Append(CmdIDs::DeleteRClick, "Remove");
	
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

	this->draggedReposEle = this->gridInst->Grid()->GetDashboardAtPixel(clickPos);
	if(this->draggedReposEle != nullptr)
	{
		this->dragMode = MouseDragMode::DragElement;
		_this->CaptureMouse();
	
		wxPoint eleOrigin = 
			this->draggedReposEle->CellPos() * this->gridInst->GridCellSize();
	
		this->draggOffset = evt.GetPosition() - eleOrigin;
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

	// Draw the document outline.
	dc.SetPen(wxColour(0, 0, 0));
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
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

	for( const DashboardElement* ele : *this->gridInst->Grid())
	{ 
		// If in the middle of a drag repositioning, draw visual
		// feedback to user.
		if(this->draggedReposEle == ele)
		{
			dc.SetPen(wxColour(0, 255, 0));
			dc.SetBrush(wxColour(50, 200, 50));
			dc.DrawRectangle(ele->PixelPos() - scroll, ele->PixelSize());
			dc.SetBrush(*wxTRANSPARENT_BRUSH);
		}
		else if(this->drawOutlineMode != GridBoundsDrawMode::Invisible)
		{ 
			dc.SetPen(outlinePen);
			dc.DrawRectangle(ele->PixelPos() - scroll, ele->PixelSize());
		}

		// Instead of having static text labels, we're going to
		// manually be in charge of drawing on the canvas.
		wxPoint textPos = ele->PixelPos();
		textPos.x += 10;
		textPos.y += 2;
		dc.DrawText(ele->Label(), textPos - scroll);
	}


	if(this->dragMode == MouseDragMode::PalleteInsert)
	{ 
		dc.SetPen(*wxTRANSPARENT_PEN);

		// Arbitrary for now
		static const int defCellHeight = 5;
		static const int defCellWidth = 20;

		wxPoint cell = (wxGetMousePosition() - _this->GetScreenPosition() + scroll) / GRIDCELLSIZE;

		if (this->gridInst->Grid()->InDocumentBounds(
			cell,
			wxSize(defCellWidth, defCellHeight)))
		{
			dc.SetBrush(wxBrush(wxColour(200, 255, 200)));
		}
		else
			dc.SetBrush(wxBrush(wxColour(255, 200, 200)));


		wxPoint pixelPos = cell * this->gridInst->GridCellSize();

		dc.DrawRectangle(
			pixelPos - scroll, 
			wxSize(
				this->gridInst->GridCellSize() * defCellWidth, 
				this->gridInst->GridCellSize() * defCellHeight));
	}
	else if(this->dragMode == MouseDragMode::DragElement)
	{
		// If we're dragging the mouse the reposition an element
		assert(this->draggedReposEle != nullptr);

		dc.SetPen(*wxTRANSPARENT_PEN);

		const int gridCellSz = this->gridInst->GridCellSize();
		wxPoint cell = wxGetMousePosition() - _this->GetScreenPosition() - this->draggOffset;
		cell = (cell/gridCellSz);
		wxPoint pixelPos = cell * gridCellSz;
		
		dc.SetBrush(wxBrush(wxColour(200, 255, 200)));
		dc.DrawRectangle(
			pixelPos - scroll, 
			this->draggedReposEle->CellSize() * gridCellSz);
	}
}

void PaneDashboard::Canvas_OnLeftUp(wxMouseEvent& evt)
{
	// Remember the handler is for this->canvasWin, not PaneDashboard, it's just
	// being redirected for the PaneDashboard to handle.
	wxScrolledWindow * _this = this->canvasWin;

	if(_this->HasCapture())
		_this->ReleaseMouse();

	switch(this->dragMode)
	{
	// PalleteInsert Handled in OnEndParamDrag()
	//case MouseDragMode::PalleteInsert:
	//	break;

	case MouseDragMode::DragElement:
		// Is an DashboardElement was being dragged, see if we can move it.
		if(this->draggedReposEle != nullptr)
		{
			wxPoint offsetPt = evt.GetPosition() - this->draggOffset;
			wxPoint cellMove = offsetPt / this->gridInst->GridCellSize();

			bool moved = 
				this->gridInst->MoveCell(
					this->draggedReposEle,
					cellMove,
					this->draggedReposEle->CellSize());

			if(moved)
			{ 
				rootWin->BroadcastDashDoc_EleMoved(
					this->gridInst->Grid(), 
					this->draggedReposEle);
			}
			else
			{
				// Immediately redraw to get rid of drag preview on the canvas.
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
	this->draggedReposEle = nullptr;
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

void PaneDashboard::OnDashDoc_NewElement(DashboardGrid* grid, DashboardElement* newEle)
{
	if(this->gridInst->Grid() != grid)
		return;

	this->gridInst->Implement(newEle);
	this->Refresh();
}

void PaneDashboard::OnDashDoc_RemElement(DashboardGrid* grid, DashboardElement* removedEle)
{
	if(this->gridInst->Grid() != grid)
		return;

	// No need to delete the actual dashboard element. This function 
	// is being called because something else is already deleting it.
	// Just remove the attached UI.
	this->gridInst->RemoveElement(removedEle, false);
	this->Refresh();
}

void PaneDashboard::OnDashDoc_ReposElement(DashboardGrid* grid, DashboardElement* modEle)
{
	if(this->gridInst->Grid() != grid)
		return;

	this->gridInst->MatchEleInstLayout(modEle);
	this->canvasWin->Refresh();
}

void PaneDashboard::OnDashDoc_MovedElement(DashboardGrid* grid, DashboardElement* movedEle)
{
	if(this->gridInst->Grid() != grid)
		return;

	this->gridInst->MatchEleInstLayout(movedEle);
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
	assert(this->rightClickedEle != nullptr);

	if(this->gridInst->Grid()->Remove(this->rightClickedEle) == false)
	{
		assert(!"Could not successfully remove node.");
		return;
	}
	
	if (this->gridInst->RemoveElement(this->rightClickedEle, false))
	{
		this->rootWin->BroadcastDashDoc_EleRem(
			this->gridInst->Grid(), 
			this->rightClickedEle);
	}
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