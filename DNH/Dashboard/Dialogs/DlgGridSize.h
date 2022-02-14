#pragma once
#include <wx/wx.h>
#include <wx/spinctrl.h>

/// <summary>
/// A modal pop-up dialog that can respecify the width and
/// height of a dashboard grid.
/// </summary>
class DlgGridSize : public wxDialog
{
public:
	/// <summary>
	/// Unique IDs for the various child UI elements 
	/// in the dialog.
	/// </summary>
	enum class IDs
	{
		SpinX,
		SpinY
	};
private:
	/// <summary>
	/// Spin control to edit the dashboard grid's width.
	/// </summary>
	wxSpinCtrl* spinX;

	/// <summary>
	/// Spin control to edit the dashboard grid's height.
	/// </summary>
	wxSpinCtrl* spinY;

public:
	DlgGridSize(wxWindow* parent, const wxSize& startingSz);

public:
	/// <summary>
	/// The dimensions of the dashboard grid being worked on.
	/// </summary>
	wxSize size;

	void OnSpinX(wxSpinEvent& evt);	// Callback for when spinfield SpinX is modified.
	void OnSpinY(wxSpinEvent& evt);	// Callback for when spinfield SpinY is modified.

	wxDECLARE_EVENT_TABLE();
};