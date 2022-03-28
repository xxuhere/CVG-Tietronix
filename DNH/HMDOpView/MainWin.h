#pragma once

#include <wx/wx.h>
#include <wx/glcanvas.h>
#include "GLWin.h"
#include <map>
#include "States/BaseState.h"

class MainWin: public wxFrame
{
private:

    GLWin* innerGLWin = nullptr;

    // We may want to consider putting these state machine
    // elements in a specialized class.
    BaseState* curState = nullptr;
    std::map<BaseState::AppState, BaseState*> states;

public:
    inline BaseState* CurrState()
    { return this->curState; }

public:
    MainWin(const wxString& title, const wxPoint& pos, const wxSize& size);

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