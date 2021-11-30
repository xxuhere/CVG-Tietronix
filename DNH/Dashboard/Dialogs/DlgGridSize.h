#pragma once
#include <wx/wx.h>
#include <wx/spinctrl.h>

class DlgGridSize : public wxDialog
{
public:
	enum class IDs
	{
		SpinX,
		SpinY
	};
private:
	wxSpinCtrl* spinX;
	wxSpinCtrl* spinY;

public:
	DlgGridSize(wxWindow* parent, const wxSize& startingSz);

public:
	wxSize size;

	void OnSpinX(wxSpinEvent& evt);
	void OnSpinY(wxSpinEvent& evt);

	wxDECLARE_EVENT_TABLE();
};