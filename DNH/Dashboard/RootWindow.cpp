#include "RootWindow.h"

#include <fstream>

#include <wx/event.h>
#include <wx/filedlg.h>
#include <nlohmann/json.hpp>
#include "ParseUtils.h"
#include "Params/ParamUtils.h"

#include "FullscreenDash.h"
#include "PaneBusLog.h"
#include "PaneInspector.h"
#include "PaneDashboard.h"


#include "defines.h"

wxDEFINE_EVENT(cvgWSEVENT, wxCommandEvent);

wxBEGIN_EVENT_TABLE(RootWindow, wxFrame)
    EVT_SIZE(RootWindow::OnResize)
    EVT_BUTTON((int)RootWindow::IDs::Connection,        RootWindow::OnButton_Connection     )
    EVT_BUTTON((int)RootWindow::IDs::Toggle_Log,        RootWindow::OnButton_ToggleLog      )
    EVT_BUTTON((int)RootWindow::IDs::Toggle_Inspector,  RootWindow::OnButton_ToggleInspector)
    EVT_BUTTON((int)RootWindow::IDs::Toggle_Fullscreen, RootWindow::OnButton_ToggleFullscreen)

    EVT_CLOSE(RootWindow::OnClose)

    EVT_MENU((int)RootWindow::IDs::ViewOutline_Invisible,   RootWindow::OnMenu_OutlineInvisible )
    EVT_MENU((int)RootWindow::IDs::ViewOutline_Dotted,      RootWindow::OnMenu_OutlineDotted    )
    EVT_MENU((int)RootWindow::IDs::ViewOutline_Light,       RootWindow::OnMenu_OutlineLight     )
    EVT_MENU((int)RootWindow::IDs::ViewOutline_Heavy,       RootWindow::OnMenu_OutlineHeavy     )

    EVT_MENU((int)RootWindow::IDs::MenuNewDashView,         RootWindow::OnMenu_NewDashboardView )
    EVT_MENU(wxID_NEW,                                      RootWindow::OnMenu_New              )
    EVT_MENU(wxID_SAVE,                                     RootWindow::OnMenu_Save             )
    EVT_MENU(wxID_SAVEAS,                                   RootWindow::OnMenu_SaveAs           )
    EVT_MENU(wxID_OPEN,                                     RootWindow::OnMenu_Open             )
    EVT_MENU((int)RootWindow::IDs::Menu_LoadStartup,        RootWindow::OnMenu_OpenStartup)

    wx__DECLARE_EVT1(cvgWSEVENT, IDs::ConChange_Error,      wxCommandEventHandler(RootWindow::OnEvent_ConChange))
    wx__DECLARE_EVT1(cvgWSEVENT, IDs::ConChange_Close,      wxCommandEventHandler(RootWindow::OnEvent_ConChange))
    wx__DECLARE_EVT1(cvgWSEVENT, IDs::ConChange_Opened,     wxCommandEventHandler(RootWindow::OnEvent_ConChange))
    wx__DECLARE_EVT1(cvgWSEVENT, IDs::ConChange_OnMessage,  wxCommandEventHandler(RootWindow::OnEvent_ConChange))

wxEND_EVENT_TABLE()

RootWindow::RootWindow(const wxString& title, const wxPoint& pos, const wxSize& size)
    :   wxFrame(NULL, wxID_ANY, title, pos, size)
{
    this->_CreateMenuBar();
    
    CreateStatusBar();
    SetStatusText("CVG Dashboard");

    wxBoxSizer* rootSizer = new wxBoxSizer(wxVERTICAL);

    this->topControlbar = new wxPanel(this, -1);
    rootSizer->Add(this->topControlbar, 0, wxGROW);
    this->sizerTopPanel = new wxBoxSizer(wxHORIZONTAL);
    this->topControlbar->SetSizer(this->sizerTopPanel);
    this->btnConnection = new wxButton(this->topControlbar, IDs::Connection, "CON");
    wxStaticText * stHost = new wxStaticText(this->topControlbar, -1, " Host:", wxDefaultPosition, wxDefaultSize);
    wxStaticText * stPort = new wxStaticText(this->topControlbar, -1, " - Port:", wxDefaultPosition, wxDefaultSize);
    this->inputHost = new wxTextCtrl(this->topControlbar, IDs::Host);
    this->inputPort = new wxTextCtrl(this->topControlbar, IDs::Port);
    this->inputPort->SetMaxSize(wxSize(60, 100));
    this->sizerTopPanel->Add(this->btnConnection, 0, wxGROW);
    this->sizerTopPanel->Add(stHost, 0, wxALIGN_BOTTOM);
    this->sizerTopPanel->Add(inputHost);
    this->sizerTopPanel->Add(stPort, 0, wxALIGN_BOTTOM);
    this->sizerTopPanel->Add(this->inputPort);

    this->btnToggleInspector    = new wxButton(this->topControlbar, (int)Toggle_Inspector,  "Ins");
    this->btnToggleLog          = new wxButton(this->topControlbar, (int)Toggle_Log,        "Log");
    this->btnToggleFullscreen   = new wxButton(this->topControlbar, (int)Toggle_Fullscreen,  "FullS");
    this->sizerTopPanel->AddStretchSpacer(1);
    this->sizerTopPanel->Add(this->btnToggleInspector   );
    this->sizerTopPanel->Add(this->btnToggleLog         );
    this->sizerTopPanel->Add(this->btnToggleFullscreen  );

    this->inputPort->SetValue("5701");
    this->inputHost->SetValue("192.168.1.110");

    this->dockingRgnWin = new wxWindow(this, -1);
    rootSizer->Add(this->dockingRgnWin, 1, wxGROW);
    this->SetSizerAndFit(rootSizer);

    m_mgr.SetManagedWindow(this->dockingRgnWin);

    // The MAIN dashboard is a special window, so we bypass the 
    // AUI pane registration and add it directly.
    //
    // The starting grid, at least 1 grid must always be in the document.
    this->grids.push_back(new DashboardGrid(GRIDCELLSIZE, "default"));
    //
    this->mainDashboard = new PaneDashboard(this->dockingRgnWin, -1, this, this->grids[0]);
    this->dockedPanes.push_back(this->mainDashboard);
    this->gridPanes.push_back(this->mainDashboard);
    m_mgr.AddPane(this->mainDashboard, wxAuiPaneInfo().CenterPane());


    // Set size before adding dock pane, so the default size
    // in RegisterDockPane will actually be respected.
    this->SetSize(1024, 800);

    //  ADD DEFAULT DOCK PANES
    //
    //////////////////////////////////////////////////
    this->RegisterDockPane(new PaneBusLog(this->dockingRgnWin, -1, this), false);
    this->RegisterDockPane(new PaneInspector(this->dockingRgnWin, -1, this), false);

    m_mgr.Update();

    // If there's a "startup.cvghmi" in the same directory as the executable 
    // (or for the very least, the starting working directory) then automatically 
    // open that one.
    //////////////////////////////////////////////////
    this->LoadDocumentFromPath("startup.cvghmi", false);

    std::vector<wxAcceleratorEntry> accelerators;
    accelerators.push_back(wxAcceleratorEntry(wxACCEL_ALT, WXK_RETURN, IDs::Toggle_Fullscreen));
    
    wxAcceleratorTable accelTable(accelerators.size(), &accelerators[0]);
    this->SetAcceleratorTable(accelTable);
}

RootWindow::~RootWindow()
{
    this->m_mgr.UnInit();
}

void RootWindow::RegisterDockPane(DockedCVGPane * pane, bool updateAUI)
{
    assert(pane->_CVGWindow()->GetParent() == this->dockingRgnWin);

    this->dockedPanes.push_back(pane);
    wxString paneTitle = pane->Title().c_str();
    this->m_mgr.AddPane(pane->_CVGWindow(), wxAuiPaneInfo().Left().Name(paneTitle).Caption(paneTitle).BestSize(300, 300));

    if(pane->GetPaneType() == DockedCVGPane::PaneType::Dashboard)
        this->gridPanes.push_back((PaneDashboard*)pane);

    if(updateAUI)
        this->m_mgr.Update();
}

bool RootWindow::CloseRegistered(DockedCVGPane * pane)
{
    for(auto it = this->dockedPanes.begin(); it != this->dockedPanes.end(); ++it)
    {
        if(*it == pane)
        {
            // dockedPanes has all of the docked panes, but dashboard panes
            // also have a duplicate copy in the gridPanes vector.
            // 
            // We need to handle this first, before we destroy the window.
            if(pane->GetPaneType() == DockedCVGPane::PaneType::Dashboard)
            {
                auto itDash = std::find(
                    this->gridPanes.begin(), 
                    this->gridPanes.end(), 
                    (PaneDashboard*)pane);

                if(itDash != this->gridPanes.end())
                    this->gridPanes.erase(itDash);
            }

            wxWindow * paneWin = pane->_CVGWindow();
            this->m_mgr.DetachPane(paneWin);
            paneWin->Destroy();
            this->dockedPanes.erase(it);
            this->m_mgr.Update();

            return true;
        }
    }
    return false;
}

DashboardGrid* RootWindow::CreateNewDashDoc(const std::string& defaultName)
{
    DashboardGrid * newGrid = new DashboardGrid(GRIDCELLSIZE, "New Dashboard");
    newGrid->name = defaultName;
    this->grids.push_back(newGrid);

    this->BroadcastDashDoc_New(newGrid);

    return newGrid;
}

void RootWindow::DeleteDashDoc(DashboardGrid* delTarg)
{
    int rmIdx = -1;
    for(size_t i = 0; i < this->grids.size(); ++i)
    {
        if(this->grids[i] == delTarg)
            rmIdx = i;
    }
    if(rmIdx == -1)
        return;

    // Send out of notifications before the deletion so the subscribers
    // can receive a valid object.
    this->BroadcastDashDoc_Deleted(delTarg);
    // Delete and erase the document
    this->grids.erase(this->grids.begin() + rmIdx);
    delete delTarg;

    // But wait! There's more...
    // If all documents have been deleted, this means all non-main panes
    // will be closed and a new default pane is created.
    if(this->grids.size() == 0)
    {
        // We can't iterate over gridPanes directly, because we might
        // be calling functions that unregister stuff from it - so a copy
        // is needed.
        auto itGridPanesCopy = this->gridPanes;
        // Destroy all non-main panes
        for(int i = 0; i < itGridPanesCopy.size(); )
        {
            if(itGridPanesCopy[i] == this->MainDashboardPane())
            { 
                ++i;
                continue;
            }

            this->CloseRegistered(itGridPanesCopy[i]);
        }

        // Only the main pane should be left afterwards from calling 
        // CloseRegistered on everything else.
        assert(this->gridPanes.size() == 1);
        assert(this->gridPanes[0] == this->MainDashboardPane());

        // Create default, force the dashboard to view its only option left.
        this->grids.push_back(new DashboardGrid(GRIDCELLSIZE, "default"));
        this->BroadcastDashDoc_New(this->grids[0]);
        this->MainDashboardPane()->SwitchToDashDoc(0);
    }
}

DashboardGrid* RootWindow::DuplicateDashDoc(DashboardGrid* copyTarg)
{
    DashboardGrid* newGrid = new DashboardGrid(copyTarg);
    newGrid->name = copyTarg->name + "_Copy";
    this->grids.push_back(newGrid);

    this->BroadcastDashDoc_New(newGrid);

    return newGrid;
}


int RootWindow::GetDashDocIndex(DashboardGrid * dashDoc)
{
    for(int i = 0; i < (int)this->grids.size(); ++i)
    {
        if(this->grids[i] == dashDoc)
            return i;
    }
    return -1;
}

DashboardGrid* RootWindow::GetDashDoc(int idx)
{
    if(idx < 0 || idx >= this->grids.size())
        return nullptr;

    return this->grids[idx];
}

bool RootWindow::SendToServer(const std::string& message, bool msgBoxOnInvalid)
{
    if(this->wsCon == nullptr)
    {
        if(msgBoxOnInvalid)
            wxMessageBox("Attempt to send message to the server when not connected.", "Application Error");

        return false;
    }
    std::shared_ptr<WsClient::OutMessage> msgStrm = std::make_shared<WsClient::OutMessage>();
    (*msgStrm) << message;
    wsCon->send(msgStrm);

    return true;
}

bool RootWindow::SendToServer(const json& jsMsg, bool msgBoxOnInvalid)
{
    return this->SendToServer(jsMsg.dump(), msgBoxOnInvalid);
}

bool RootWindow::IsCanvasFullscreen()
{
    return this->fullscreenWin != nullptr;
}

void RootWindow::ToggleCanvasFullscreen(bool val)
{
    bool isfull = this->IsCanvasFullscreen();
    if(val == isfull)
        return;

    if(val)
    { 
        this->mainDashboard->DetachCanvas();

        this->fullscreenWin = 
            new FullscreenDash(this, this->mainDashboard->CanvasWin());

        this->fullscreenWin->Show();
    }
    else
    {
        this->fullscreenWin->DetachCanvas();
        this->mainDashboard->ReattachCanvas();
        this->mainDashboard->CanvasWin()->Show();
    }
}

void RootWindow::ToggleCanvasFullscreen()
{
    this->ToggleCanvasFullscreen(!this->IsFullScreen());
}

CVG::BaseEqSPtr RootWindow::ProcessEquipmentCreationJSON(const json& jsEq)
{
    std::string guid;
    std::string manufacturer;
    std::string name;
    std::string purpose;
    std::string type;
    std::string hostname;
    const json* outParams;

    // TODO: Error checking should be done here
    CVG::Equipment::ParseEquipmentFields(
        jsEq, 
        guid, 
        manufacturer, 
        name, 
        purpose, 
        type, 
        hostname,
        &outParams);

    // There must be a GUID that we don't have a collision with.
    if(guid.empty())
        return nullptr; // TODO: Raise issue

                  // Don't add duplicates.
    if(this->equipmentCache.find(guid) != this->equipmentCache.end())
        return nullptr;

    // Extra client data
    json jsClientData;
    if(outParams != nullptr)
        CVG::Equipment::ExtractClientData(jsClientData, jsEq);

    // Parse params
    CVG::EQType eqtype = CVG::ConvertToEqType(type);
    std::vector<CVG::ParamSPtr> params;
    if(outParams != nullptr)
    {
        bool paramErrs = false;
        std::string paramErr;
        for (const json& itp : *outParams)
        {
            CVG::ParamSPtr p = CVG::ParamUtils::Parse(itp, paramErr);
            if (p == nullptr)
            {
                paramErrs = true;
                break;
            }
            params.push_back(p);
        }
        if(paramErrs == true)
            return nullptr; // TODO: Raise issue
    }

    // Create and register new equipment.
    CVG::Equipment * pNewEq = new CVG::Equipment(name, manufacturer, purpose, hostname, guid, eqtype, params, jsClientData);
    CVG::BaseEqSPtr eqSPtr = CVG::BaseEqSPtr(pNewEq);
    this->equipmentCache[guid] = eqSPtr;

    for(DockedCVGPane* cvgpanes : this->dockedPanes)
        cvgpanes->_CVG_EVT_OnNewEquipment(eqSPtr); 

    return eqSPtr;
}

void RootWindow::FinishProcessingNewEquipment(std::vector<CVG::BaseEqSPtr> newEqs)
{
    // Refresh everything in case we receive something we already knew about.
    // This can happen if we get disconnected and reconnect to the same server
    // session. Note that this can even happen between app sessions if we load
    // a dashboard (they store GUIDs).
    for(DashboardGrid* grid : this->grids)
        grid->RefreshAllParamInstances(this);

    //  Check if there was any disconnected equipment 
    //  that matches the purpose.
    //////////////////////////////////////////////////

    // Get everything known in all grids
    std::map<std::string, std::string> unusedGUIDAndPurpose;
    for(DashboardGrid* dg : this->grids)
    {
        std::vector<DashboardGrid::GUIDPurposePair> eqs = dg->GetEquipmentList();
        for(DashboardGrid::GUIDPurposePair i : eqs)
            unusedGUIDAndPurpose[i.guid] = i.purpose;
    }
    // Remove everything from it that's known in the cache. Whatever is left
    // is what's unknown to the cache in the grids.
    for(auto it : this->equipmentCache)
        unusedGUIDAndPurpose.erase(it.first);

    // See if there's anything in the created equipments that
    // matches an unfilled purpose
    //////////////////////////////////////////////////
    for(CVG::BaseEqSPtr newEq : newEqs)
    {
        if(newEq->Purpose().empty())
            continue;

        for(auto it : unusedGUIDAndPurpose)
        {
            if(newEq->Purpose() != it.second)
                continue;

            // We found a match, do the purpose remapping.
            for(DashboardGrid* g : this->grids)
                g->RemapInstance(it.first, newEq->GUID(), this);

            for(DockedCVGPane* pane : this->dockedPanes)
            { 
                pane->_CVG_EVT_OnRemapEquipmentPurpose(
                    newEq->Purpose(), 
                    it.first, 
                    newEq->GUID());
            }

            unusedGUIDAndPurpose.erase(it.first);
            break;
        }
    }
}

void RootWindow::_CreateMenuBar()
{
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(wxID_NEW,      "&New\tCtrl-N");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_OPEN,                 "&Open\tCtrl-O");
    menuFile->Append(IDs::Menu_LoadStartup,     "Open Startup");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_SAVE,     "&Save...\tCtrl-S");
    menuFile->Append(wxID_SAVEAS,   "Save &As...\tCtrl-Shift-S");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    wxMenu* menuView = new wxMenu;
        wxMenu* menuViewOutline = new wxMenu;
        menuViewOutline->Append(ViewOutline_Invisible,  "Hidden");
        menuViewOutline->Append(ViewOutline_Dotted,     "Dotted");
        menuViewOutline->Append(ViewOutline_Light,      "Light");
        menuViewOutline->Append(ViewOutline_Heavy,      "Heavy");
    menuView->AppendSubMenu(menuViewOutline,            "Outlines");
    menuView->Append((int)IDs::MenuNewDashView,         "New Dashboard View");

    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuView, "&View");
    menuBar->Append(menuHelp, "&Help");
    SetMenuBar(menuBar);
}

void RootWindow::_ResetNetworkingData()
{
    this->recvdAnyEquipment = false;
    this->selfGUID.clear();
    this->equipmentCache.clear();
}

void RootWindow::MatchUIStateToConnection(UIConState cs)
{
    switch(cs)
    {
    case UIConState::Connected:
        this->btnConnection->SetLabel("D/C");
        this->inputHost->Disable();
        this->inputPort->Disable();
        break;

    case UIConState::Disconnected:
        this->btnConnection->SetLabel("CON");
        this->inputHost->Enable();
        this->inputPort->Enable();
        break;

    case UIConState::Transitory:
        this->btnConnection->SetLabel("...");
        this->inputHost->Disable();
        this->inputPort->Disable();
        break;
    }
}

void RootWindow::OnResize(wxSizeEvent& evt)
{
    this->Layout();
}

void RootWindow::BroadcastDashDoc_New(DashboardGrid * grid)
{
    for(PaneDashboard* pd : this->gridPanes)
        pd->OnDashDoc_New(grid);
}

void RootWindow::BroadcastDashDoc_Deleted(DashboardGrid * grid)
{
    for(PaneDashboard* pd : this->gridPanes)
        pd->OnDashDoc_Del(grid);
}

void RootWindow::BroadcastDashDoc_EleRepos(DashboardGrid* grid, DashboardElement* ele)
{
    for(PaneDashboard* pd : this->gridPanes)
        pd->OnDashDoc_ReposElement(grid, ele);
}

void RootWindow::BroadcastDashDoc_EleMoved(DashboardGrid* grid, DashboardElement* ele)
{
    for(PaneDashboard* pd : this->gridPanes)
        pd->OnDashDoc_MovedElement(grid, ele);
}

void RootWindow::BroadcastDashDoc_EleNew(DashboardGrid* grid, DashboardElement* ele)
{
    for(PaneDashboard* pd : this->gridPanes)
        pd->OnDashDoc_NewElement(grid, ele);
}

void RootWindow::BroadcastDashDoc_EleRem(DashboardGrid* grid, DashboardElement* ele)
{
    for(PaneDashboard* pd : this->gridPanes)
        pd->OnDashDoc_RemElement(grid, ele);
}

void RootWindow::BroadcastDashDoc_Renamed(DashboardGrid* grid)
{
    for(PaneDashboard* pd : this->gridPanes)
        pd->OnDashDoc_Renamed(grid);
}

PaneDashboard* FindDashboardDirectlyUnderMouse(const std::vector<PaneDashboard*>& allowedPanes)
{
    wxPoint mousePt = wxGetMousePosition();
    wxWindow * win = wxFindWindowAtPoint(mousePt);

    // Find if the application knows about the window the mouse pointer is over,
    // and see if it's a PaneDashboard. If it is, do functionality for when
    // an InspectorParam is being dragged over a PaneDashboard.
    if(win == nullptr)
        return nullptr;
    
    // We want to be over the child canvas, but we need to 
    wxWindow* parentWin = win->GetParent();
    if(parentWin == nullptr)
        return nullptr;

    // Sanity check, make sure it's a PaneDashboard we know about.
    const auto it = 
        std::find(
            allowedPanes.begin(), 
            allowedPanes.end(), 
            (PaneDashboard*)parentWin);

    if(it == allowedPanes.end())
        return nullptr;

    // The pane has multiple children window, make sure it's the canvas.
    PaneDashboard * draggedOver = *it;
    if(draggedOver->CanvasWin() != win)
        return nullptr;
    
    return draggedOver;
}

void RootWindow::Param_OnDragStart(const std::string& eq, CVG::ParamSPtr p)
{
    // Sanity reset - shouldn't actually be changed yet unless there was an
    // issue clearing it out on a previous drag.
    assert(this->draggedDashboard == nullptr);

    Param_OnDragMotion(eq, p);
}

void RootWindow::Param_OnDragEnd(const std::string& eq, CVG::ParamSPtr p)
{
    if(this->draggedDashboard != nullptr)
        this->draggedDashboard->OnEndParamDrag(eq, p);
    
    this->draggedDashboard = nullptr;
}

void RootWindow::Param_OnDragCancel()
{
    if(this->draggedDashboard != nullptr)
        this->draggedDashboard->OnCancelParamDrag();
    
    this->draggedDashboard = nullptr;
    // !TODO: Reimplement
    // this->dashboard->Refresh();
    // this->dashboard->OnCancelParamDrag();
}

void RootWindow::Param_OnDragMotion(const std::string& eq, CVG::ParamSPtr p)
{
    PaneDashboard* draggedOver = FindDashboardDirectlyUnderMouse(this->gridPanes);
    
    if(this->draggedDashboard != nullptr && draggedOver == this->draggedDashboard)
    {
        // If we're dragging over what we were previously dragging.
        //
        this->draggedDashboard->OnParamDrag(eq, p);
        return;
    }
    else if(draggedOver == nullptr)
    {
        // There wasn't a DashboardPane under the dragged mouse.
        //
        if(this->draggedDashboard != nullptr)
        {
            // If we're not dragging over anything but previously were, let the old
            // dashboard know it's not being dragged over anymore.
            //
            this->draggedDashboard->OnCancelParamDrag();
            this->draggedDashboard = nullptr;
        }
        return;
    }

    // There was a DashboardPane under the mouse when there previously
    // was none.
    this->draggedDashboard = draggedOver;
    this->draggedDashboard->OnStartParamDrag(eq, p);
}

void RootWindow::OnEvent_ConChange(wxCommandEvent& event)
{
    switch(event.GetId())
    {
    case IDs::ConChange_Error:
        if(this->wsClient != nullptr)
        { 
            this->wsClient->stop();
            if(this->wsCon != nullptr)
            {
                this->wsCon->send_close(0, "Closing on error");
                this->wsCon.reset();
            }

            this->wsClientThread.join();
            this->wsClient.reset();

            for(DockedCVGPane * pane : this->dockedPanes)
                pane->_CVG_EVT_OnDisconnect();
        }
        this->MatchUIStateToConnection(UIConState::Disconnected);
        break;

    case IDs::ConChange_Close:

        this->_ResetNetworkingData();

        if(this->wsClient != nullptr)
        { 
            this->wsClient->stop();
            if(this->wsClientThread.joinable())
                this->wsClientThread.join();

            this->wsClient.reset();
            this->wsCon.reset();
        }

        for(DockedCVGPane * pane : this->dockedPanes)
            pane->_CVG_EVT_OnDisconnect();
        
        this->MatchUIStateToConnection(UIConState::Disconnected);
        break;

    case IDs::ConChange_OnMessage:
        {
            // local thread's copy.
            std::queue<std::string> msgs;
            {
                std::lock_guard<std::mutex> lock(this->wsmsgsMutex);
                // Move semantics to move it out of wsMessages into msg, so it's
                // not in a thread contentious variable.
                msgs = this->wsMessages;
                // We have ownership of the data to process it in this thread, we
                // can clear out the thread's copy now.
                this->wsMessages = std::queue<std::string>();
            }

            while(!msgs.empty())
            {
                std::string curMsg = msgs.front();
                msgs.pop();

                for(DockedCVGPane * pane : this->dockedPanes)
                    pane->_CVG_OnMessage(curMsg);

                json js = json::parse(curMsg, nullptr, false);
                if(!this->ProcessJSONMessage(js))
                    continue;

                if(!js.is_null())
                { 
                    for(DockedCVGPane * pane : this->dockedPanes)
                        pane->_CVG_OnJSON(js);
                }
            }

            
        }
        break;

    case IDs::ConChange_Opened:
        {
            this->_ResetNetworkingData();
            this->MatchUIStateToConnection(UIConState::Connected);

            // On connection, register as an equipment to get access to
            // listen on the data bus.
            json jsreg;
            jsreg["apity"] = "register";
            jsreg["type"] = "spectator";
            jsreg["name"] = "CVG Dashboard";
            jsreg["manufacturer"] = "WU-STL/Tietronix";

            std::shared_ptr<WsClient::OutMessage> ss = std::make_shared<WsClient::OutMessage>();
            (*ss) << jsreg.dump();
            this->wsCon->send(ss);

            for(DockedCVGPane * pane : this->dockedPanes)
                pane->_CVG_EVT_OnConnect();
        }
        break;
    }
}

void RootWindow::OnClose(wxCloseEvent& event)
{
    if(this->wsClient != nullptr)
    {
        this->wsClient->stop();
        if(this->wsClientThread.joinable())
            this->wsClientThread.join();
        this->wsClient = nullptr;
    }
    this->Destroy();
}

void RootWindow::OnButton_Connection(wxCommandEvent& event)
{
    if(this->wsClient != nullptr)
    {
        if(this->wsCon != nullptr)
            this->wsCon->send_close(0, "User requested disconnect.");

        this->wsClient->stop();
        this->MatchUIStateToConnection(UIConState::Transitory);
    }
    else
    {
        std::string host = this->inputHost->GetValue().ToStdString();
        std::string port = this->inputPort->GetValue().ToStdString();

        // Make the connection
        std::string fullDst = host + ":" + port + "/realtime";
        this->wsClient = std::shared_ptr<WsClient>(new WsClient(fullDst));

        // Set up the callbacks
        this->wsClient->on_close =
            [this](
                std::shared_ptr<WsClient::Connection> con, 
                int status, const 
                std::string & reason)
            {
                wxCommandEvent event( cvgWSEVENT, IDs::ConChange_Close);
                this->GetEventHandler()->AddPendingEvent( event );
            };

        this->wsClient->on_error = 
            [this](
                std::shared_ptr<WsClient::Connection> con, 
                const SimpleWeb::error_code &ec) 
            {
                wxCommandEvent event( cvgWSEVENT, IDs::ConChange_Error);
                this->GetEventHandler()->AddPendingEvent( event );
            };

        this->wsClient->on_message = 
            [this]( 
                std::shared_ptr<WsClient::Connection> con, 
                std::shared_ptr<WsClient::InMessage> msg)
            {
                wxCommandEvent event( cvgWSEVENT, IDs::ConChange_OnMessage);
                { // scope for mutex guard
                    std::lock_guard<std::mutex> guard(this->wsmsgsMutex);
                    std::stringstream sstrm;
                    sstrm << msg->rdbuf();
                    this->wsMessages.push(sstrm.str());
                }
                this->GetEventHandler()->AddPendingEvent( event );
            };

        this->wsClient->on_open = 
            [this]( std::shared_ptr<WsClient::Connection> con )
            {
                wxCommandEvent event( cvgWSEVENT, IDs::ConChange_Opened);
                this->wsCon = con;
                this->GetEventHandler()->AddPendingEvent( event );
            };

        this->MatchUIStateToConnection(UIConState::Transitory);

        // Start the connection process.
        //
        // The websocket client is blocking, so it requires its own thread.
        this->wsClientThread = 
            std::thread([this]{ this->wsClient->start(); });
    }
}

void RootWindow::OnButton_ToggleLog(wxCommandEvent& evt)
{
    for(DockedCVGPane * d : this->dockedPanes)
    {
        if(d->Title() == "Bus Log")
        {
            this->CloseRegistered(d);
            return;
        }
    }

    this->RegisterDockPane(new PaneBusLog(this->dockingRgnWin, -1, this));
}

void RootWindow::OnButton_ToggleInspector(wxCommandEvent& evt)
{
    for(DockedCVGPane * d : this->dockedPanes)
    {
        if(d->Title() == "Equipment Inspector")
        {
            this->CloseRegistered(d);
            return;
        }
    }
    
    this->RegisterDockPane(new PaneInspector(this->dockingRgnWin, -1, this));
}

void RootWindow::OnButton_TogglePresetBar(wxCommandEvent& evt)
{
}

void RootWindow::OnButton_ToggleFullscreen(wxCommandEvent& evt)
{
    this->ToggleCanvasFullscreen();
}

void RootWindow::OnMenu_OutlineInvisible(wxCommandEvent& evt)
{
    for(PaneDashboard* pd : this->gridPanes)
    { 
        pd->SetDrawDashboardOutline(
            PaneDashboard::GridBoundsDrawMode::Invisible);
    }
}

void RootWindow::OnMenu_OutlineDotted(wxCommandEvent& evt)
{
    for(PaneDashboard* pd : this->gridPanes)
    { 
        pd->SetDrawDashboardOutline(
            PaneDashboard::GridBoundsDrawMode::Dotted);
    }
}

void RootWindow::OnMenu_OutlineLight(wxCommandEvent& evt)
{
    for(PaneDashboard* pd : this->gridPanes)
    { 
        pd->SetDrawDashboardOutline(
            PaneDashboard::GridBoundsDrawMode::Light);
    }
}

void RootWindow::OnMenu_OutlineHeavy(wxCommandEvent& evt)
{
    for(PaneDashboard* pd : this->gridPanes)
    { 
        pd->SetDrawDashboardOutline(
            PaneDashboard::GridBoundsDrawMode::Heavy);
    }
}

void RootWindow::OnMenu_NewDashboardView(wxCommandEvent& evt)
{
    int idView = this->mainDashboard->GetDashDocIndex();

    // Sanity check
    if(idView == -1)
        return;

    this->RegisterDockPane(
        new PaneDashboard(
            this->dockingRgnWin, 
            -1, 
            this,
            this->grids[idView]));
}

void RootWindow::OnMenu_New(wxCommandEvent& evt)
{
    this->ClearDocument();

    assert(this->grids.size() == 0);

    DashboardGrid* newDash = new DashboardGrid(GRIDCELLSIZE, "default");
    this->grids.push_back(newDash);
    this->mainDashboard->SwitchToDashDoc(0);

    this->BroadcastDashDoc_New(newDash);
}

void RootWindow::OnMenu_Save(wxCommandEvent& evt)
{
    if(!this->documentFullPath.empty())
    { 
        this->SaveDocumentAs(this->documentFullPath);
        return;
    }
    this->OnMenu_SaveAs(evt);
}

void RootWindow::OnMenu_SaveAs(wxCommandEvent& evt)
{
    wxFileDialog saveDialog(
        this, 
        _("Save CVG HMI"), 
        "", 
        "",
        "CVG HMI files (*.cvghmi)|*.cvghmi", 
        wxFD_SAVE);

    int ret = saveDialog.ShowModal();
    if(ret != wxID_OK)
        return;

    std::string savePath = 
        saveDialog.GetPath().ToStdString();

    this->SaveDocumentAs(savePath);
}

void RootWindow::OnMenu_Open(wxCommandEvent& evt)
{
    wxFileDialog openDialog(
        this, 
        _("Load CVG HMI"), 
        "", 
        "",
        "CVG HMI files (*.cvghmi)|*.cvghmi", 
        wxFD_OPEN|wxFD_FILE_MUST_EXIST);

    int ret = openDialog.ShowModal();
    if(ret != wxID_OK)
        return;

    std::string openPath = 
        openDialog.GetPath().ToStdString();

    this->LoadDocumentFromPath(openPath, true);
}

void RootWindow::OnMenu_OpenStartup(wxCommandEvent& evt)
{
    this->LoadDocumentFromPath("startup.cvghmi", true);
}

json RootWindow::DocumentAsJSON()
{
    DashboardGrid * mainGrid = this->mainDashboard->GetGrid();

    json doc;

    json jsDashboards = json::array();
    for(DashboardGrid* dg : this->grids)
    {
        json jsCurDash;
        jsCurDash["main"]   = (dg == mainGrid);
        jsCurDash["name"]   = dg->name;
        jsCurDash["width"]  = dg->CellWidth();
        jsCurDash["height"] = dg->CellHeight();
        jsCurDash["cellsz"] = dg->GridCellSize();

        json jsEles = json::array();
        for(DashboardElement* ele: *dg)
        {
            json jse;
            jse["posx"]     = ele->CellPos().x;
            jse["posy"]     = ele->CellPos().y;
            jse["dimx"]     = ele->CellSize().x;
            jse["dimy"]     = ele->CellSize().y;
            jse["param"]    = ele->ParamID();
            jse["guid"]     = ele->EqGUID();
            jse["uiimpl"]   = ele->GetUIImplName();
            jse["label"]    = ele->Label();
            jse["type"]     = CVG::ConvertToString(ele->Param()->Type());

            jsEles.push_back(jse);
        }
        jsCurDash["elements"] = jsEles;
        jsDashboards.push_back(jsCurDash);
    }
    doc["dashboards"] = jsDashboards;

    // Another pass to get unique equipments and their
    // purposes.
    //
    // The purposes need to be tracked in case equipments 
    // need to be remapped.
    //
    // (Arguably this could be rolled into the loop above,
    // but feels cleaner seperately without any significant
    // overhead)
    std::set<std::string> guidsAlreadySeen;
    json jsEquips;
    for(DashboardGrid* dg : this->grids)
    {
        for(DashboardElement * ele : *dg)
        {
            std::string guid = ele->EqGUID();
            auto it = guidsAlreadySeen.find(guid);
            if(it != guidsAlreadySeen.end())
                continue;

            jsEquips[guid] = ele->EqPurpose();
        }
    }
    doc["equipments"] = jsEquips;

    return doc;
}

bool RootWindow::SaveDocumentAs(const std::string& filePath)
{
    std::ofstream fileStream(filePath);
    if(!fileStream.is_open())
    {
        wxMessageBox("Saving error", "Could not write to specified file.");
        return false;
    }

    for(DockedCVGPane* pane : this->dockedPanes)
        pane->_CVG_Session_SavePre();

    json layoutData = this->DocumentAsJSON();
    fileStream << layoutData.dump();

    this->documentDirty = 0;
    this->documentFullPath = filePath;
    // TODO: Get filename portion and set titlebar.

    for(DockedCVGPane* pane : this->dockedPanes)
        pane->_CVG_Session_SavePost();
    return true;
}

bool RootWindow::LoadDocumentFromPath(const std::string& filePath, bool dlgOnErr)
{
    std::ifstream fileStream(filePath);
    if(!fileStream.is_open())
    {
        if(dlgOnErr)
            wxMessageBox("Loading error", "Could not read specified file.");

        return false;
    }

    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    std::string fileDataAsStr = buffer.str();

    json jsDoc = json::parse(fileDataAsStr);
    return this->LoadDocument(jsDoc);
}

bool RootWindow::LoadDocument(const json& js, bool clearFirst)
{ 
    for(DockedCVGPane* pane : this->dockedPanes)
        pane->_CVG_Session_OpenPre(clearFirst);

    if(clearFirst == true)
        this->ClearDocument();

    if(!js.contains("equipments") || !js["equipments"].is_object())
        return false;

    if(!js.contains("dashboards") || !js["dashboards"].is_array())
        return false;


    // We store the purposes of the GUIDs in case the GUIDs don't exist
    // anymore, then we could try to connect to some other equipment
    // with the same parameters that matches the purpose.
    std::map<std::string, std::string> mapLoadedGuidToPurpose;
    const json& jsEquips = js["equipments"];
    for(
        json::const_iterator it = jsEquips.cbegin();
        it != jsEquips.cend();
        ++it)
    {
        mapLoadedGuidToPurpose[it.key()] = (std::string)it.value();
    }

    // The grid that should be assigned to the main dashboard panel.
    DashboardGrid * mainGrid = nullptr;

    // Record the dashboard grids that were added. While it might seem that we 
    // could just use this->grids, remember this may be more than what was just
    // loaded if clearFirst is false.
    std::vector<DashboardGrid*> addedGrids;

    // Create the dashboards and their individual elements
    const json& jsDashboards = js["dashboards"];
    for(const json& jsd : jsDashboards)
    {
        std::string dashName = jsd["name"];
        int width = jsd["width"];
        int height = jsd["height"];
        int cellsz = jsd["cellsz"];

        DashboardGrid* grid = new DashboardGrid(cellsz, dashName);
        this->grids.push_back(grid);
        // TODO: Set cell width and height of grid

        const json& jsEles = jsd["elements"];
        for(const json& jse : jsEles)
        {
            int posx            = jse["posx"];
            int posy            = jse["posy"];
            int dimx            = jse["dimx"];
            int dimy            = jse["dimy"];
            std::string paramId = jse["param"];
            std::string guid    = jse["guid"];
            std::string uiImpl  = jse["uiimpl"];
            std::string label   = jse["label"];
            std::string strTy   = jse["type"];
            CVG::DataType ty    = CVG::ConvertToDataType(strTy);

            if(ty == CVG::DataType::Unknown)
                continue;
            
            // Create a placeholder param of the correct type. It will be 
            // replaced (hopefully) with an element of the correct type 
            // when refreshed later.
            CVG::ParamSPtr ptr;
            switch(ty)
            {
            case CVG::DataType::Bool:
                ptr = CVG::ParamSPtr(new CVG::ParamBool(paramId, label, "", "", false, boost::none, boost::none));
                break;
            case CVG::DataType::Enum:
                ptr = CVG::ParamSPtr(new CVG::ParamEnum(paramId, label, "", "", "Unknown", boost::none, boost::none, std::vector<std::string>{"Unknown"}));
                break;
            case CVG::DataType::Float:
                ptr = CVG::ParamSPtr(new CVG::ParamFloat(paramId, label, "", "", 0.0, boost::none, boost::none, boost::none, boost::none));
                break;
            case CVG::DataType::Int:
                ptr = CVG::ParamSPtr(new CVG::ParamInt(paramId, label, "", "", 0, boost::none, boost::none, boost::none, boost::none));
                break;
            case CVG::DataType::String:
                ptr = CVG::ParamSPtr(new CVG::ParamString(paramId, label, "", "", "", boost::none, boost::none));
                break;

            case CVG::DataType::Event:
                ptr = CVG::ParamSPtr(new CVG::ParamEvent(paramId, label, "", "", false, false));
                break;

            default:
                assert(!"Unhandled type in document load.");
            }

            grid->AddDashboardElement(
                guid, 
                mapLoadedGuidToPurpose[guid], 
                ptr,
                posx,
                posy,
                uiImpl,
                dimx,
                dimy);
        }

        if(jsd.contains("main") && (bool)jsd["main"] == true)
            mainGrid = grid;

        addedGrids.push_back(grid);
    }

    // Make the new documents known.
    for(size_t i = 0; i < this->grids.size(); ++i)
        this->BroadcastDashDoc_New(this->grids[i]);
    
    if(clearFirst == true)
    { 
        // There needs to be a document shown in the main panel, this leads to 
        // 3 possible cases that needs to be handled.
        //
        // This only needs to be done if we're not appending.
        if(this->grids.size() == 0)
        {
            // Case 1: No documents were loaded, this means we need to manually
            // create a default document.
            DashboardGrid* defGrid = new DashboardGrid(GRIDCELLSIZE, "default");
            this->grids.push_back(defGrid);
            this->mainDashboard->SwitchToDashDoc(0);

        }
        else if(mainGrid == nullptr)
        {
            // Case 2: No starting document was defined. This means we just default to
            // the first one found.
            this->mainDashboard->SwitchToDashDoc(0);
        }
        else
        {
            // Case 3: Use the defined document.
            int idx = this->GetDashDocIndex(mainGrid);
            this->mainDashboard->SwitchToDashDoc(idx);
        }
    }

    // Take the initiative to remap things if we don't have a GUID for them, but 
    // where the equipment purposes match.
    //////////////////////////////////////////////////

    // Get a list of unconnected equipment
    // Get a connection of current equipment and purpose
    // Require where needed.
    std::map<std::string, std::string> purposeToGUID;
    std::map<std::string, std::string> guidToPurpose;
    for(auto it: this->equipmentCache)
    {
        purposeToGUID[it.second->Purpose()] = it.first;
        guidToPurpose[it.first] = it.second->Purpose();
    }

    // Find out all the GUIDs from the loaded document that don't have a GUID
    // active for them.
     
    // Get all GUIDs loaded
    std::set<std::string> unusedGUIDs;
    unusedGUIDs.erase("");
    //
    for(auto it: mapLoadedGuidToPurpose)
        unusedGUIDs.insert(it.first);
    // Remove anything we know about in the cache.
    for(auto it : guidToPurpose)
        unusedGUIDs.erase(it.second);
        
    // For anything we don't have an equipment for, see if we have a matching 
    // purpose - and use that GUID instead
    for(const std::string& unused : unusedGUIDs)
    {
        auto newMappingForUnused = purposeToGUID.find(unused);
        if(newMappingForUnused == purposeToGUID.end())
            continue;

        // For now, we're just going to brute force remap for all loaded
        // documents.
        for(DashboardGrid* grid : addedGrids)
            grid->RemapInstance(unused, newMappingForUnused->second, this);
    }

    //////////////////////////////////////////////////

    // The loaded DashboardGrid elements are created with placeholder Params.
    // Get the elements to grab the active cached versions.
    //
    // Note that some may already may be been refreshed from the RemapInstance()
    // directly above - this is a little redundant, but more-than-acceptable
    // overhead.
    for(DashboardGrid* grid : addedGrids)
        grid->RefreshAllParamInstances(this);
    
    for(DockedCVGPane* pane : this->dockedPanes)
        pane->_CVG_Session_OpenPost(clearFirst);

    return true;
}

void RootWindow::ClearDocument()
{
    for(DockedCVGPane* pane : this->dockedPanes)
        pane->_CVG_Session_ClearPre();

    // All the dashboards panes (except the main one) will be closed down
    // anyways when we start closing everything, so might as well preemptively
    // close them.
    std::vector<PaneDashboard*> gridPanesCpy = this->gridPanes;
    for(PaneDashboard* pd : gridPanesCpy)
    {
        if(pd == this->mainDashboard)
            continue;

        this->CloseRegistered(pd);
    }

    for(DashboardGrid* dg : this->grids)
        delete dg;

    this->grids.clear();

    this->documentFullPath.clear();
    this->documentDirty = 0;

    // Whatever invoked ClearDocument() is expected to know that the UI is
    // left in a delecate situation. The main dashboard pane needs to have
    // a default document set or else if will crash the program when it tries
    // to access its (now deleted) refferenced DashboardGrid.

    for(DockedCVGPane* pane : this->dockedPanes)
        pane->_CVG_Session_ClearPost();
}

bool RootWindow::ProcessJSONMessage(const json& js)
{
    if(!js.contains("apity"))
        return false;

    const json& jsapity = js["apity"];
    if(!jsapity.is_string())
        return false;

    std::string strAPI = jsapity;

    // If registration just (successfully) finished, send a query
    // for equipment.
    if(strAPI == "register")
    {
        // We need to validate things are as expected, and also instantly request an
        // equipment list.
        std::string strSuc;
        if(!CVG::ParseUtils::ExtractJSONString(js, "status", strSuc) || strSuc != "success")
        {
            this->wsClient->stop();
            wxMessageBox("Did not receive notice of successful registration", "Handshake error");
            return false;
        }

        std::string strGUID;
        if(!CVG::ParseUtils::ExtractJSONString(js, "guid", strGUID) || strGUID.empty() )
        {
            this->wsClient->stop();
            wxMessageBox("Did not receive self GUID from registration.", "Handshake error");
            return false;
        }
        this->selfGUID = strGUID;

        // Send a query for the equipment listing.
        json jsEqRequest;
        jsEqRequest["apity"] = "equipment";
        this->SendToServer(jsEqRequest);

    }
    else if(strAPI == "equipment")
    {
        this->recvdAnyEquipment = true;

        if(!js.contains("equipment") || !js["equipment"].is_array())
            return false;

        // Store newly added equipment for purpose remapping.
        std::vector<CVG::BaseEqSPtr> newEqs;

        // This is going to be very similar how to the server loads the SEquipments. 
        // Certain parts of the logic have been unified with how the server loads
        // Equipment, but there are still redundancies to allow the Server to have
        // extra features added in without affecting the CVGData portion.
        const json & jsEqs = js["equipment"];
        for(const json& jseq : jsEqs)
        { 
            CVG::BaseEqSPtr newEq = this->ProcessEquipmentCreationJSON(jseq);
            if(newEq == nullptr)
                continue;

            newEqs.push_back(newEq);
        }
        this->FinishProcessingNewEquipment(newEqs);

        
    }
    else if(strAPI == "valset" || strAPI == "changedval")
    {
        if(!js.contains("guid") || !js.contains("sets"))
            return false;

        std::string eqGUID = js["guid"];

        auto eqFind = this->equipmentCache.find(eqGUID);
        if(eqFind == this->equipmentCache.end())
            return false;

        CVG::BaseEqSPtr eq = eqFind->second;

        const json& jssets = js["sets"];
        for(json::const_iterator it = jssets.cbegin(); it != jssets.cend(); ++it)
        {
            CVG::ParamSPtr param = eq->GetParam(it.key());
            if(param == nullptr)
                continue;

            const json& jsSet = it.value();
            if(!jsSet.contains("status") || jsSet["status"] != "success" || !jsSet.contains("val"))
                continue;

            if(param->SetValue(jsSet["val"]) == CVG::SetRet::Success)
            {
                for(DockedCVGPane * cvgp : this->dockedPanes)
                    cvgp->_CVG_EVT_OnParamChange(eq, param);
            }
        }
    }
    else if(strAPI == "changedroster")
    {
        if(!js.contains("change") || !js["change"].is_string())
            return false;

        std::string change = js["change"];
        std::string guid = js["guid"];

        if(change == "rem")
        {
            auto itEqFind = this->equipmentCache.find(guid);
            if(itEqFind == this->equipmentCache.end())
                return false;

            for(DockedCVGPane* pane : this->dockedPanes)
                pane->_CVG_EVT_OnRemEquipment(itEqFind->second);

            this->equipmentCache.erase(itEqFind);
            return true;
        }
        else if(change == "add")
        {

            // The vector will only have 1 item, but it keeps the logic unified
            // with how the "equipment" handler creates and manages new Equipments.
            //
            //std::vector<CVG::BaseEqSPtr> newEqs;
            //CVG::BaseEqSPtr newEq = this->ProcessEquipmentCreationJSON(js);
            //if(newEq == nullptr)
            //    return false;
            //
            //newEqs.push_back(newEq);
            //this->FinishProcessingNewEquipment(newEqs);

            // The changedroster message doesn't give us all the information we need,
            // so for now we'll just download the entire equipment roster and look
            // for new stuff in it.
            json jsEqRequest;
            jsEqRequest["apity"] = "equipment";
            this->SendToServer(jsEqRequest);
        }
        else
            return false;
    }
    else if(strAPI == "ping")
    {} // Eat it up
    else if(!this->recvdAnyEquipment)
    {
        // Ideally we want an equipment message before anything else, but this might
        // be a fragile way to try to enforce that. It's very possible messages might
        // slip in before we get our initial equipment message.
        wxMessageBox(
            "Received network message before equipment information was received.", 
            "Invalid Handshaking");
    }

    return true;
}

CVG::BaseEqSPtr RootWindow::CVGB_GetEquipment(const std::string& eq)
{
    auto it = this->equipmentCache.find(eq);
    if(it == this->equipmentCache.end())
        return nullptr;

    return it->second;
}

bool RootWindow::CVGB_Submit(const std::string& eq, const std::string& param)
{
    auto it = this->equipmentCache.find(eq);
    
    json jsSubmit;
    jsSubmit["apity"] = "valset";
    jsSubmit["guid"] = eq;

    json jsSetDict;
    jsSetDict[param] = "submit";
    jsSubmit["sets"] = jsSetDict;

    return this->SendToServer(jsSubmit);
}

void RootWindow::CVGB_SetValue(const std::string& eqGUID, const std::string& param, const std::string& value)
{
    json js;
    js["apity"] = "valset";
    js["guid"] = eqGUID;

    json jssets;
    jssets[param] = value;
    js["sets"] = jssets;

    this->SendToServer(js);
}

bool RootWindow::CVGB_GetValue(const std::string& eqGUID, const std::string& param, std::string& outVal)
{
    outVal.clear();

    auto it = this->equipmentCache.find(eqGUID);
    if(it == this->equipmentCache.end())
        return false;

    CVG::ParamSPtr ptr = it->second->GetParam(param);
    if(ptr == nullptr)
        return false;

    return ptr->GetValue(outVal);
}

void RootWindow::OnExit(wxCommandEvent& event)
{
    Close(true);
}
void RootWindow::OnAbout(wxCommandEvent& event)
{
    wxMessageBox(
        "WU-STL CVG",
        "WU-STL CVG", 
        wxOK | wxICON_INFORMATION);
}
void RootWindow::OnHello(wxCommandEvent& event)
{
    wxLogMessage("");
}