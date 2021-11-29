#pragma once
#include <wx/wx.h>
class RootWindow;

class FullscreenDash : public wxFrame
{
private:
	RootWindow * parent;
	wxWindow * child;
	wxBoxSizer * canvasSizer;
public:
	enum class IDs
	{
		Toggle_Fullscreen
	};

	FullscreenDash(RootWindow * parent, wxWindow * child);
	void DetachCanvas();
	void OnResize(wxSizeEvent& evt);
	void OnToggleFullscreen(wxCommandEvent& evt);

protected:
	DECLARE_EVENT_TABLE()
};