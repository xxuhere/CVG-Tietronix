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

class MainWin: public wxFrame
{
private:

    GLWin* innerGLWin = nullptr;

    // We may want to consider putting these state machine
    // elements in a specialized class.
    BaseState* curState = nullptr;
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

public:
    MainWin(const wxString& title, const wxPoint& pos, const wxSize& size);

    /// <summary>
    /// Clear all records of snapshot requests
    /// </summary>
    void ClearWaitingSnaps();

    /// <summary>
    /// Do some housekeeping with request tokens and state information
    /// that's expected to run every-so often. Right now it's invoked
    /// from GLWin to run once a frame.
    /// </summary>
    void PerformMaintenenceCycle();

    /// <summary>
    /// Clear all records of snapshot requests that have finished.
    /// </summary>
    void ClearFinishedSnaps();

    void ClearFinishedRecordings();

    /// <summary>
    /// Request a snapshot and cache the request in waitingSnaps.
    /// </summary>
    /// <returns></returns>
    SnapRequest::SPtr RequestSnap(int idx, const std::string& prefix);

    VideoRequest::SPtr RecordVideo(int idx, const std::string& prefix);

    bool IsRecording(int idx);

    bool StopRecording(int idx);

    void ReloadAppOptions();

    void PopulateStates();
    void States_Initialize();
    void States_AppShutdown();
    bool ChangeState(BaseState::AppState newState);

    void OnFocus(wxFocusEvent& evt);
    void OnResize(wxSizeEvent& evt);
    void OnExit(wxCommandEvent& evt);

protected:
    wxDECLARE_EVENT_TABLE();
};