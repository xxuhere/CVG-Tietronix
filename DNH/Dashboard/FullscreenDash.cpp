#include "RootWindow.h"
#include "FullscreenDash.h"

BEGIN_EVENT_TABLE(FullscreenDash, wxFrame)
	EVT_SIZE(FullscreenDash::OnResize)
	EVT_MENU((int)IDs::Toggle_Fullscreen,  FullscreenDash::OnToggleFullscreen)
END_EVENT_TABLE()

FullscreenDash::FullscreenDash(RootWindow * parent, wxWindow* child)
	: wxFrame(parent, -1, "Fullscreen", wxDefaultPosition, wxDefaultSize, wxSTAY_ON_TOP|wxFRAME_NO_TASKBAR)
{
	this->child = child;
	this->parent = parent;

	this->child->Reparent(this);
	this->child->Show();

	this->canvasSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(this->canvasSizer);
	this->canvasSizer->Add(child, 1, wxGROW);

	this->Maximize();

	std::vector<wxAcceleratorEntry> accelerators;
	accelerators.push_back(wxAcceleratorEntry(wxACCEL_ALT, WXK_RETURN, (int)IDs::Toggle_Fullscreen));

	wxAcceleratorTable accelTable(accelerators.size(), &accelerators[0]);
	this->SetAcceleratorTable(accelTable);
}

void FullscreenDash::DetachCanvas()
{
	this->canvasSizer->Detach(0);
}

void FullscreenDash::OnResize(wxSizeEvent& evt)
{
	this->Layout();
}

void FullscreenDash::OnToggleFullscreen(wxCommandEvent& evt)
{
	// Get the parent window to close us and take the canvas back.
	this->parent->ToggleCanvasFullscreen(false);
}