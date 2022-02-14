#pragma once
#include <wx/wx.h>
class RootWindow;

/// <summary>
/// The fullscreen container for a dashboard.
/// 
/// The UI for a dashboard is the canvas that has menu content 
/// surrounding it. And around that is even more UI containers
/// such as the AUI windowing and the main application. To remove
/// all this stuff when the application is fullscreened, the 
/// canvas is reparented into a bare-boned parent that is fullscreened
/// without any top-level window decorations.
/// </summary>
class FullscreenDash : public wxFrame
{
private:
	/// <summary>
	/// The Root window of the application. Used to handshake
	/// transfer of the canvas.
	/// </summary>
	RootWindow * parent;

	/// <summary>
	/// The child canvas to fill up the UI realestate of this window.
	/// </summary>
	wxWindow * child;

	/// <summary>
	/// The sizer so manual resizing doesn't need to be performed.
	/// 
	/// (This turns out to not quite to be the cas as manual resizing
	/// is still performed because of how Linux behaves).
	/// </summary>
	wxBoxSizer * canvasSizer;
public:

	enum class IDs
	{
		/// <summary>
		/// Keyboard accelerator ID to exit fullscreen.
		/// </summary>
		Toggle_Fullscreen
	};

	FullscreenDash(RootWindow * parent, wxWindow * child);

	/// <summary>
	/// Detach the child canvas (child) in preparation for 
	/// reparenting to the fullscreen.
	/// </summary>
	void DetachCanvas();

	void OnResize(wxSizeEvent& evt);
	void OnToggleFullscreen(wxCommandEvent& evt);	// Handler for Toggle_Fullscreen.

protected:
	DECLARE_EVENT_TABLE()
};