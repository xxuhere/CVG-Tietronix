#pragma once

#include <wx/wx.h>
#include <wx/glcanvas.h>

class DashboardCamInst;

/// <summary>
/// UI class for the contents of a DashboardCamInst
/// </summary>
class DashboardCamInstUI : public wxWindow
{
public:
	static wxGLContext * sharedContext;

public:
	DashboardCamInst * instOwner;

	wxGLCanvas * displayWin = nullptr;

public:
	DashboardCamInstUI(wxWindow* parent, DashboardCamInst* instOwner);

public:
	void Redraw();
	static bool ShutdownSystem();

public:
	void OnConnectionDirty(wxCommandEvent& evt);

	DECLARE_EVENT_TABLE();
};