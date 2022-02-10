#pragma once
#include <Simple-WebSocket-Server/client_ws.hpp>
#include <wx/wx.h>
#include <wx/aui/aui.h>
#include <string>
#include <queue>
#include <thread>
#include <memory>
#include <vector>

#include <Equipment.h>
#include "DockedCVGPane.h"
#include "CVGBridge.h"
#include "DashDragCont.h"

class FullscreenDash;
class PaneDashboard;
class DashboardGrid;
class DashboardTile;
typedef SimpleWeb::SocketClient<SimpleWeb::WS> WsClient;

/// <summary>
/// The top-most application window.
/// </summary>
class RootWindow : 
    public wxFrame,
    public CVGBridge
{
public:

    /// <summary>
    /// Event and window IDs.
    /// 
    /// Not all of these need to be unique to each other as there are different
    /// ID spaces defined here, but it makes it easier to keep track of if they're
    /// all unique.
    /// </summary>
    enum IDs
    {
        /// <summary>
        /// The connecting host text input field.
        /// </summary>
        Host,

        /// <summary>
        /// The connecting port text input field.
        /// </summary>
        Port,

        /// <summary>
        /// The Connect/Disconnect button.
        /// </summary>
        Connection,

        /// <summary>
        /// Custom message ID for when the WS connection received an error.
        /// </summary>
        ConChange_Error,

        /// <summary>
        /// Custom message ID for when the WS connection closes.
        /// </summary>
        ConChange_Close,

        /// <summary>
        /// Custom message ID for when the WS is successfully opened.
        /// This should not be confused with successfully registering with the DNH.
        /// </summary>
        ConChange_Opened,

        /// <summary>
        /// Custom message ID for when the WS receives a message.
        /// 
        /// One UI message is sent for every 
        /// </summary>
        ConChange_OnMessage,

        /// <summary>
        /// The button that toggles viewing the log panel.
        /// </summary>
        Toggle_Log,

        /// <summary>
        /// The button that toggles viewing the inspector.
        /// </summary>
        Toggle_Inspector,

        /// <summary>
        /// The button that toggles between fullscreen operational and
        /// windowed edit mode.
        /// </summary>
        Toggle_Fullscreen,

        /// <summary>
        /// The button to add a new dashboard.
        /// </summary>
        MenuNewDashView,

        // Buttons to change the dashboard view.
        ViewOutline_Invisible,
        ViewOutline_Dotted,
        ViewOutline_Light,
        ViewOutline_Heavy,

        Menu_LoadStartup
    };

    /// <summary>
    /// Used to change the UI of things, whos UI state are based off 
    /// the connection status.
    /// </summary>
    enum UIConState
    {
        /// <summary>
        /// The application is connected.
        /// </summary>
        Connected,

        /// <summary>
        /// The application is disconnected.
        /// </summary>
        Disconnected,

        /// <summary>
        /// The application is in an unknown or transitioning state, such as 
        /// being in progress of connecting or disconnecting.
        /// </summary>
        Transitory
    };

private:
    /// <summary>
    /// The wxWidget docking window manager.
    /// </summary>
    wxAuiManager m_mgr;

    /// <summary>
    /// The window where stuff will be docked. While m_mgr will manage them, this needs
    /// to be exposed so things know where to parent new panel windows to.
    /// </summary>
    wxWindow * dockingRgnWin    = nullptr;

    /// <summary>
    /// When connected, the GUID of ourself.
    /// </summary>
    std::string selfGUID;

    /// <summary>
    /// The menu bar at the top containing all the connection UI widgets.
    /// </summary>
    wxPanel * topControlbar     = nullptr;
    
    /// <summary>
    /// The sizer to manage the contents of the topControlbar.
    /// </summary>
    wxSizer * sizerTopPanel     = nullptr;

    /// <summary>
    /// The button to connect/disconnect.
    /// </summary>
    wxButton * btnConnection    = nullptr;

    /// <summary>
    /// The button to toggle viewing the inspector pane.
    /// </summary>
    wxButton * btnToggleInspector;

    /// <summary>
    /// The button to toggle viewing the log pane.
    /// </summary>
    wxButton * btnToggleLog;

    wxButton * btnToggleFullscreen;

    /// <summary>
    /// The text input field to specify the host to connect to.
    /// </summary>
    wxTextCtrl * inputHost      = nullptr;

    /// <summary>
    /// The text input field to specify the port to connect to.
    /// </summary>
    wxTextCtrl * inputPort      = nullptr;

    /// <summary>
    /// The registry of panes currently active in the application.
    /// </summary>
    std::vector<DockedCVGPane*> dockedPanes;

    /// <summary>
    /// The thread managing the WebSockets connection.
    /// </summary>
    std::thread wsClientThread;

    /// <summary>
    /// The message connection for the web socket connection. 
    /// (While wsCon represents the actual connection, it does not provide an
    /// 
    /// </summary>
    std::shared_ptr<WsClient> wsClient;

    /// <summary>
    /// The web socket connection.
    /// </summary>
    std::shared_ptr<WsClient::Connection> wsCon;

    /// <summary>
    /// The mutex guarding wsMessages from being concurrently read/written by 
    /// multiple threads concurrently.
    /// </summary>
    std::mutex wsmsgsMutex;

    /// <summary>
    /// The web messages queued from being processed byt the application.
    /// 
    /// We don't handle these immediately because when we first receive them, its 
    /// in the WebSockets thread, but the sane thing to do is to process them in
    /// the main/UI thread.
    /// </summary>
    std::queue<std::string> wsMessages;

    /// <summary>
    /// The primary dashboard
    /// </summary>
    PaneDashboard* mainDashboard;

    /// <summary>
    /// Check if any Equipment queries have been received
    /// since connection.
    /// </summary>
    bool recvdAnyEquipment = false;

    /// <summary>
    /// When dragging from somewhere into any of the multiple dashboards, 
    /// keep track of which is currently being dragged into.
    /// </summary>
    PaneDashboard* draggedDashboard;

    /// <summary>
    /// All the dashboards in the open document
    /// </summary>
    std::vector<DashboardGrid*> grids;

    /// <summary>
    /// All the views into dashboards docked to the window.
    /// 
    /// Note that mainDashboard will also be included, but should not EVER 
    /// be closed or removed unless the app session ends.
    /// </summary>
    std::vector<PaneDashboard*> gridPanes;

    std::string documentFullPath;

    std::string title;

    /// <summary>
    /// Specifies if the document is dirty. A value of 0 means the document is clean.
    /// The integer value specifies how many changes have been recorded.
    /// </summary>
    int documentDirty = 0;

    /// <summary>
    /// The window used for the fullscreen display. We use another toplevel
    /// derived window instead of finagling the RootWindow because it's easier.
    /// </summary>
    FullscreenDash * fullscreenWin;

    UIConState lastState = UIConState::Disconnected;

public:
    // THIS SHOULDN't BE PUBLIC! WE'RE BREAKING ENCAPSULATION
    // FOR NOW.
    //
    /// <summary>
    // Client-side synced copy of the equipments.
    /// </summary>
    std::map<std::string, CVG::BaseEqSPtr> equipmentCache;

    /// <summary>
    /// The thing being dragged from an inspector.
    /// </summary>
    DashDragCont globalInspectorDrag;

public:

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="title">Application titlebar text.</param>
    /// <param name="pos">Default position.</param>
    /// <param name="size">Default size.</param>
    RootWindow(const wxString& title, const wxPoint& pos, const wxSize& size);

    ~RootWindow();

    /// <summary>
    /// Registers a DockedCVGPane subclass to the application.
    /// </summary>
    /// <param name="pane">The pane to register.</param>
    void RegisterDockPane(DockedCVGPane * pane, bool updateAUI = true);

    /// <summary>
    /// Close and unregister a docked pane.
    /// 
    /// The pane being closed must have previously been successfully registered.
    /// </summary>
    /// <param name="copyTarg">The pane to close.</param>
    /// <returns>True if successful, else false.</returns>
    bool CloseRegistered(DockedCVGPane * pane);

    std::string GUID() const
    { return this->selfGUID; }

    PaneDashboard * MainDashboardPane()
    { return this->mainDashboard; }

    /// <summary>
    /// Send a string to the connected WebSockets server.
    /// </summary>
    /// <param name="message">The message to send.</param>
    /// <param name="msgBoxOnInvalid">
    /// If true, raise a modal message box if a connection error is detected.
    /// </param>
    /// <returns>True if successfully sent. Else, false.</returns>
    bool SendToServer(const std::string& message, bool msgBoxOnInvalid = true);

    /// <summary>
    /// Send a JSON object to the connected WebSockets server.
    /// </summary>
    /// <param name="jsMsg">The payload to send.</param>
    /// <param name="msgBoxOnInvalid">
    /// If true, raise a modal message box if a connection error is detected.
    /// </param>
    /// <returns>True if successfully sent. Else, false.</returns>
    bool SendToServer(const json& jsMsg, bool msgBoxOnInvalid = true);

    DashboardGrid* CreateNewDashDoc(const std::string& defaultName);
    void DeleteDashDoc(DashboardGrid* delTarg);
    DashboardGrid* DuplicateDashDoc(DashboardGrid* copyTarg);
    size_t DashdocCount() const
    { return this->grids.size(); }

    int GetDashDocIndex(DashboardGrid * dashDoc);
    DashboardGrid* GetDashDoc(int idx);

    bool IsCanvasFullscreen();
    void ToggleCanvasFullscreen(bool val);
    void ToggleCanvasFullscreen();

    //////////////////////////////////////////////////

    void BroadcastDashDoc_New(          DashboardGrid* grid);
    void BroadcastDashDoc_Deleted(      DashboardGrid* grid);
    void BroadcastDashDoc_EleRepos(     DashboardGrid* grid,    DashboardTile* tile);
    void BroadcastDashDoc_EleResize(    DashboardGrid* grid,    DashboardTile* tile);
    void BroadcastDashDoc_EleMoved(     DashboardGrid* grid,    DashboardTile* tile);
    void BroadcastDashDoc_EleNew(       DashboardGrid* grid,    DashboardTile* tile);
    void BroadcastDashDoc_EleRem(       DashboardGrid* grid,    DashboardTile* tile);
    void BroadcastDashDoc_EleRelabled(  DashboardGrid* grid,    DashboardTile* tile);
    void BroadcastDashDoc_Renamed(      DashboardGrid* grid);
    void BroadcastDashDoc_Resized(      DashboardGrid* grid);


private:
    /// <summary>
    /// Call to process JSON information defining an equipment to be reified.
    /// </summary>
    /// <param name="jsEq">The JSON object holding the equipment information.</param>
    CVG::BaseEqSPtr ProcessEquipmentCreationJSON(const json& jsEq);

    /// <summary>
    /// This should be call after ProcessEquipmentCreationJSON(). It only needs to be
    /// called once if a batch of ProcessEquipmentCreationJSON() are called, but should
    /// be called before the end of the frame.
    /// </summary>
    /// <param name="newEqs">All the new equipments that were recently created.</param>
    void FinishProcessingNewEquipment(std::vector<CVG::BaseEqSPtr> newEqs);

    /// <summary>
    /// Creation of the menubar is in its own dedicated function.
    /// </summary>
    void _CreateMenuBar();

    /// <summary>
    /// Clears out all network session data.
    /// </summary>
    void _ResetNetworkingData();

    /// <summary>
    /// Update relevant UI to a connection state change.
    /// </summary>
    /// <param name="cs">The new connection state.</param>
    void MatchUIStateToConnection(UIConState cs);
    
    /// <summary>
    /// Process received JSON message at the app level.
    /// 
    /// This function is also in charge of making sure the incomming JSON
    /// passes a minimum level of "correctness" before other stuff is allowed
    /// to parse it.
    /// 
    /// Checks
    ///     - js["apity"] is valid.
    /// </summary>
    /// <param name="js">The JSON to analyze.</param>
    /// <returns>If true, allow panes to also process the JSON.</returns>
    bool ProcessJSONMessage(const json& js);

    // To remove, event handlers from the "Hello World!" sample the project 
    // started from. These will be removed eventually if we do not have a
    // need for them as a reference. (wleu 11/16/2021)
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    /// Window resize handler.
    void OnResize(wxSizeEvent& evt);
    
    // Websocket handlers.
    //
    // WebSocket callbacks (for wsClient) redirect to these functions on the 
    // main thread. This is done to make sure there isn't threading issues
    // between the WebSocket and the UI.
    void OnEvent_ConChange(wxCommandEvent& event);

    // Callback when the application closes.
    // The callback makes sure the connection is closed and the connection
    // thread is jointed.
    void OnClose(wxCloseEvent& event);

    // Callback for the Connect/Disconnect button.
    void OnButton_Connection(wxCommandEvent& event);

    void OnButton_ToggleLog(wxCommandEvent& evt);

    void OnButton_ToggleInspector(wxCommandEvent& evt);

    void OnButton_TogglePresetBar(wxCommandEvent& evt);

    void OnButton_ToggleFullscreen(wxCommandEvent& evt);

    void OnMenu_OutlineInvisible(wxCommandEvent& evt);
    void OnMenu_OutlineDotted(wxCommandEvent& evt);
    void OnMenu_OutlineLight(wxCommandEvent& evt);
    void OnMenu_OutlineHeavy(wxCommandEvent& evt);

    void OnMenu_NewDashboardView(wxCommandEvent& evt);

    void OnMenu_New(wxCommandEvent& evt);
    void OnMenu_Save(wxCommandEvent& evt);
    void OnMenu_SaveAs(wxCommandEvent& evt);
    void OnMenu_Open(wxCommandEvent& evt);
    void OnMenu_OpenStartup(wxCommandEvent& evt);

    void OnAccelerator_Fullscreen(wxCommandEvent& evt);

    //////////////////////////////////////////////////

    /// <summary>
    /// Get the current document session as a JSON representation.
    /// </summary>
    json DocumentAsJSON();

    /// <summary>
    /// Save the current document session as a JSON file.
    /// </summary>
    bool SaveDocumentAs(const std::string& filePath);

    /// <summary>
    /// Load the contents of a JSON at a specified file.
    /// </summary>
    bool LoadDocumentFromPath(const std::string& filePath, bool dlgOnErr = true);

    /// <summary>
    /// Load the contents of a JSON document.
    /// </summary>
    bool LoadDocument(const json& js, bool clearFirst = true);

    /// <summary>
    /// Clear the document session, destroying all currently
    /// loaded DocumentGrids as well as any attachment to a file
    /// session.
    /// </summary>
    void ClearDocument();

    inline void FlagDocumentDirty()
    { ++this->documentDirty; }

    inline bool IsDocumentDirty() const
    { return this->documentDirty != 0;}

    //  VIRTUAL CVGBridge FUNCTIONS
    //////////////////////////////////////////////////

    // CVGBridge implemenations for when an InspectorParam or other
    // Inspector element is being dragged.
    void Param_OnDragStart(const std::string& eq, DashDragCont dc) override;
    void Param_OnDragEnd(const std::string& eq, DashDragCont dc) override;
    void Param_OnDragCancel() override;
    void Param_OnDragMotion(const std::string& eq, DashDragCont dc) override;

    CVG::BaseEqSPtr CVGB_GetEquipment(const std::string& eq) override;
    bool CVGB_Submit(const std::string& eq, const std::string& param) override;
    void CVGB_SetValue(const std::string& eqGUID, const std::string& param, const std::string& value) override;
    bool CVGB_GetValue(const std::string& eqGUID, const std::string& param, std::string& outVal) override;

    wxDECLARE_EVENT_TABLE();
};