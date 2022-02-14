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
	/// <summary>
	/// The shared drawing context for all 
	/// DashboardCamInstUI objects.
	/// </summary>
	static wxGLContext * sharedContext;

public:

	/// <summary>
	/// The camera tile instance the DashboardCamInstUI
	/// is showing a video feed for.
	/// </summary>
	DashboardCamInst * instOwner;

	/// <summary>
	/// The OpenGL canvas displaying the video feed.
	/// </summary>
	wxGLCanvas * displayWin = nullptr;

public:

	DashboardCamInstUI(wxWindow* parent, DashboardCamInst* instOwner);

public:

	/// <summary>
	/// Redraw the contents of the OpenGL window to show
	/// the latest video feed.
	/// </summary>
	void Redraw();

	/// <summary>
	/// Shutdown the entire system. This should be called
	/// at the end of the application's life.
	/// </summary>
	/// <returns>True if success.</returns>
	static bool ShutdownSystem();

public:

	/// <summary>
	/// Custom message triggered in the streaming camera thread.
	/// </summary>
	/// <param name="evt"></param>
	void OnConnectionDirty(wxCommandEvent& evt);

	DECLARE_EVENT_TABLE();
};