
#include "RootWindow.h"
#include <wx/event.h>
#include <nlohmann/json.hpp>
#include "ParseUtils.h"
#include "Params/ParamUtils.h"

#include "PaneBusLog.h"
#include "PaneInspector.h"
#include "PaneDashboard.h"

wxDEFINE_EVENT(cvgWSEVENT, wxCommandEvent);

wxBEGIN_EVENT_TABLE(RootWindow, wxFrame)
EVT_SIZE(RootWindow::OnResize)
EVT_BUTTON((int)RootWindow::IDs::Connection, RootWindow::OnButton_Connection)
EVT_CLOSE(RootWindow::OnClose)
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
    rootSizer->Add(this->topControlbar);
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

    this->inputPort->SetValue("5701");
    this->inputHost->SetValue("192.168.1.110");

    this->dockingRgnWin = new wxWindow(this, -1);
    rootSizer->Add(this->dockingRgnWin, 1, wxGROW);
    this->SetSizerAndFit(rootSizer);

    m_mgr.SetManagedWindow(this->dockingRgnWin);

    // The dashboard is a special window, so we bypass the AUI pane registration
    // and add it directly.
    PaneDashboard* dashboard = new PaneDashboard(this->dockingRgnWin, -1, this);
    this->dockedPanes.push_back(dashboard);
    m_mgr.AddPane(dashboard, wxAuiPaneInfo().CenterPane());


    // Set size before adding dock pane, so the default size
    // in RegisterDockPane will actually be respected.
    this->SetSize(1024, 800);

    //  ADD DEFAULT DOCK PANES
    //
    //////////////////////////////////////////////////
    this->RegisterDockPane(new PaneBusLog(this->dockingRgnWin, -1, this));
    this->RegisterDockPane(new PaneInspector(this->dockingRgnWin, -1, this));

    m_mgr.Update();
}

RootWindow::~RootWindow()
{
    this->m_mgr.UnInit();
}

void RootWindow::RegisterDockPane(DockedCVGPane * pane)
{
    assert(pane->_CVGWindow()->GetParent() == this->dockingRgnWin);

    this->dockedPanes.push_back(pane);
    wxString paneTitle = pane->Title().c_str();
    this->m_mgr.AddPane(pane->_CVGWindow(), wxAuiPaneInfo().Left().Name(paneTitle).Caption(paneTitle).BestSize(300, 300));
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

void RootWindow::_CreateMenuBar()
{
    wxMenu* menuFile = new wxMenu;
    //menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
    //menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
    //    "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
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

void RootWindow::Param_OnDragStart(const std::string& eq, CVG::ParamSPtr p)
{
}

void RootWindow::Param_OnDragEnd(const std::string& eq, CVG::ParamSPtr p)
{
}

void RootWindow::Param_OnDragCancel()
{
}

void RootWindow::Param_OnDragMotion(const std::string& eq, CVG::ParamSPtr p)
{
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
                        pane->_CVG_OnJSON(curMsg);
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
        wsCon->send_close(0, "User requested disconnect.");
        this->MatchUIStateToConnection(UIConState::Transitory);
        //this->wsClient->stop();
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

        // This is going to be very similar how to the server loads the SEquipments. 
        // Certain parts of the logic have been unified with how the server loads
        // Equipment, but there are still redundancies to allow the Server to have
        // extra features added in without affecting the CVGData portion.
        const json & jsEqs = js["equipment"];
        for(const json& jseq : jsEqs)
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
                jseq, 
                guid, 
                manufacturer, 
                name, 
                purpose, 
                type, 
                hostname,
                &outParams);

            // There must be a GUID that we don't have a collision with.
            if(guid.empty())
                continue; // TODO: Raise issue

            // Don't add duplicates.
            if(this->equipmentCache.find(guid) != this->equipmentCache.end())
                continue;

            // Extra client data
            json jsClientData;
            if(outParams != nullptr)
                CVG::Equipment::ExtractClientData(jsClientData, jseq);

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
                    continue; // TODO: Raise issue
            }

            // Create and register new equipment.
            CVG::Equipment * pNewEq = new CVG::Equipment(name, manufacturer, purpose, hostname, guid, eqtype, params, jsClientData);
            CVG::BaseEqSPtr eqSPtr = CVG::BaseEqSPtr(pNewEq);
            this->equipmentCache[guid] = eqSPtr;

            for(DockedCVGPane* cvgpanes : this->dockedPanes)
                cvgpanes->_CVG_EVT_OnNewEquipment(eqSPtr);        
        }
        
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

            if(param->SetValue(jsSet["val"]))
            {
                for(DockedCVGPane * cvgp : this->dockedPanes)
                    cvgp->_CVG_EVT_OnParamChange(eq, param);
            }
        }
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