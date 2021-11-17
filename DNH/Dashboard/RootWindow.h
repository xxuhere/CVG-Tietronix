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
        ConChange_OnMessage
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
    // Client-side synced copy of the equipments.
    /// </summary>
    std::map<std::string, CVG::BaseEqSPtr> equipmentCache;

    /// <summary>
    /// Check if any Equipment queries have been received
    /// since connection.
    /// </summary>
    bool recvdAnyEquipment = false;
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
    void RegisterDockPane(DockedCVGPane * pane);

    std::string GUID() const
    { return this->selfGUID; }

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

private:
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

    //  VIRTUAL CVGBridge FUNCTIONS
    //////////////////////////////////////////////////

    /// CVGBridge implemenations for when an InspectorParam is being dragged.
    void Param_OnDragStart(const std::string& eq, CVG::ParamSPtr p) override;
    void Param_OnDragEnd(const std::string& eq, CVG::ParamSPtr p) override;
    void Param_OnDragCancel() override;
    void Param_OnDragMotion(const std::string& eq, CVG::ParamSPtr p) override;

    void CVGB_SetValue(const std::string& eqGUID, const std::string& param, const std::string& value) override;
    bool CVGB_GetValue(const std::string& eqGUID, const std::string& param, std::string& outVal) override;

    //////////////////////////////////////////////////

    wxDECLARE_EVENT_TABLE();
};