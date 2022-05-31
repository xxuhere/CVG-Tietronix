#pragma once

#include <wx/wx.h>
#include <wx/sound.h>
#include <wx/glcanvas.h>
#include "GLWin.h"
#include <map>
#include "States/BaseState.h"

#include "Hardware/HardwareMgr.h"
#include "Hardware/LaserSys.h"
#include "OpSession.h"
#include "CamVideo/SnapRequest.h"
#include "CamVideo/VideoRequest.h"

/// <summary>
/// The main application top-level window.
/// </summary>
class MainWin: public wxFrame
{
private:

    /// <summary>
    /// interior OpenGL rendering viewport.
    /// </summary>
    GLWin* innerGLWin = nullptr;

    // We may want to consider putting these state machine
    // elements in a specialized class.
    BaseState* curState = nullptr;

    /// <summary>
    /// A mapping of all state available to an AppState id.
    /// </summary>
    std::map<BaseState::AppState, BaseState*> states;

    /// <summary>
    /// Snapshot filename counter.
    /// </summary>
    int snapCtr = 0;

    /// <summary>
    /// Video recording counter.
    /// </summary>
    int videoCtr = 0;

public:
    inline BaseState* CurrState()
    { return this->curState; }

    HardwareMgr hmgr;
    LaserSys * hwLaser = nullptr;

    /// <summary>
    /// Operation session data.
    /// </summary>
    OpSession opSession;

    /// <summary>
    /// The record of snapshots waiting to be fullfilled.
    /// </summary>
    std::vector<SnapRequest::SPtr> waitingSnaps;

    /// <summary>
    /// Handles to video recordings that are currently active.
    /// </summary>
    std::vector<VideoRequest::SPtr> recordingVideos;

    /// <summary>
    /// Sound to be played when a snapshot request is performed.
    /// </summary>
    wxSound cameraSnap;

    wxSound doubleBeep;

public:
    MainWin(const wxString& title, const wxPoint& pos, const wxSize& size);

    //////////////////////////////////////////////////
    //
    //      SESSION UTILS
    //
    //////////////////////////////////////////////////

    bool InitializeSession();

    //////////////////////////////////////////////////
    //
    //      MISC
    //
    //////////////////////////////////////////////////

    /// <summary>
    /// Do some housekeeping with request tokens and state information
    /// that's expected to run every-so often. Right now it's invoked
    /// from GLWin to run once a frame.
    /// </summary>
    void PerformMaintenenceCycle();

    //////////////////////////////////////////////////
    //
    //      MEDIA CAPTURE UTILITIES
    //
    //////////////////////////////////////////////////
    // A lot of what's in this category is delegating media capturing 
    // functionality of CamStreamMgr to the CamStreamMgr singleton. At the
    // application level, these functions should be used instead as they will
    // notify the application to manage some extra events and state changes -
    // such as: 
    //  - letting the application know when to update graphical representations
    //      of if video is being recorded or a snapshot being taken.
    //  - playing feedback audio when a picture is taken.
    //  - etc.

    /// <summary>
    /// Clear all records of snapshot requests
    /// </summary>
    void ClearWaitingSnaps();

    /// <summary>
    /// Clear all records of snapshot requests that have finished.
    /// </summary>
    void ClearFinishedSnaps();

    /// <summary>
    /// Clear all records of video requests that aren't pending or active.
    /// </summary>
    void ClearFinishedRecordings();

    /// <summary>
    /// Request a snapshot and cache the request in waitingSnaps.
    /// </summary>
    /// <returns>The SnapRequest from CamStreamMgr.</returns>
    SnapRequest::SPtr RequestSnap(int idx, const std::string& prefix);

    std::vector<SnapRequest::SPtr> RequestSnapAll(const std::string& prefix);

    /// <summary>
    /// Set a camera to start saving to a video file.
    /// </summary>
    /// <param name="idx">The index of the video to record</param>
    /// <param name="prefix">The prefix to apply to the video.</param>
    /// <returns>The video request from CamStreamMgr.</returns>
    VideoRequest::SPtr RecordVideo(int idx, const std::string& prefix);

    /// <summary>
    /// Query if a video is recording.
    /// 
    /// Accessibility function that delegates to CamStreamMgr.
    /// </summary>
    /// <param name="idx">The id of the camera to record.</param>
    /// <returns>True if the camera successfully started recording to file.</returns>
    bool IsRecording(int idx);

    /// <summary>
    /// Stop recording video from a camera.
    /// </summary>
    /// <param name="idx">Id of the camera to stop recording from.</param>
    /// <returns>True if the request was successful.</returns>
    bool StopRecording(int idx);

    //////////////////////////////////////////////////
    //
    //      OPTIONS UTILITIES
    //
    //////////////////////////////////////////////////

    /// <summary>
    /// Reload app options and integrate them into the application. Certain
    /// options will be immediately 
    /// </summary>
    void ReloadAppOptions();

    //////////////////////////////////////////////////
    //
    //      APPLICATION STATE MACHINE STUFF
    //
    //////////////////////////////////////////////////

    /// <summary>
    /// Populates and initializes the state machine. This will be called
    /// by GLWin the first time it draw.
    /// 
    /// Originally this was done in the MainWin constructor, but because
    /// of OpenGL management issues on Linux, it's currently easier to
    /// defer this initialization to GLWin.
    /// </summary>
    void InitializeAppStateMachine();

    /// <summary>
    /// Create the applications states and store them into this->states.
    /// </summary>
    void PopulateStates();

    /// <summary>
    /// Call Initialize() for all states in the state machine system.
    /// </summary>
    void States_Initialize();

    /// <summary>
    /// Call ClosingApp() for all states in the state machine system.
    /// </summary>
    void States_AppShutdown();

    /// <summary>
    /// Switch the active state.
    /// </summary>
    /// <param name="newState">The id of the state to change to.</param>
    /// <returns>
    /// Returns true if the state was found and successfully switched to.
    /// </returns>
    bool ChangeState(BaseState::AppState newState);

    //////////////////////////////////////////////////
    //
    //      WXWIDGETS MESSAGE HANDLERS
    //
    //////////////////////////////////////////////////

    void OnFocus(wxFocusEvent& evt);        // Keyboard focus change
    void OnResize(wxSizeEvent& evt);
    void OnExit(wxCommandEvent& evt);

    void OnAccelerator_SaveCurOptions(wxCommandEvent& evt);

protected:

    std::string EnsureAndGetCapturesFolder() const;

protected:
    wxDECLARE_EVENT_TABLE();
};