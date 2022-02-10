#pragma once

#include <wx/wx.h>
#include <string>
#include "../CamChannel.h"

class PaneInspector;
class CVGBridge;

class InsExtWebCamera : public wxWindow
{
public:

	CVGBridge * bridge;
	std::string eqGUID;
	CamChannel camChan;

	wxStaticText * staticText;
	wxStaticBitmap * icon;

	wxColour origBGColor;

public:
	inline void SetOriginalBGColor()
	{
		this->SetBackgroundColour(this->origBGColor);
		this->Refresh();
	}

	inline void SetDraggedBGColor()
	{
		this->SetBackgroundColour(wxColour(200, 255, 200));
		this->Refresh();
	}

public:

	InsExtWebCamera(wxWindow * parent, const std::string & eqGUID, CVGBridge * bridge, const CamChannel & camChan);

	void OnMotion(wxMouseEvent& evt);
	void OnLeftButtonDown(wxMouseEvent& evt);
	void OnLeftButtonUp(wxMouseEvent& evt);
	void OnMouseCaptureChanged(wxMouseCaptureChangedEvent& evt);
	void OnMouseCaptureLost(wxMouseCaptureLostEvent& evt);

	wxDECLARE_EVENT_TABLE();
};