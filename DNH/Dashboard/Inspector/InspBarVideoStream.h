#pragma once

#include <wx/wx.h>
#include <string>
#include "../CamChannel.h"

class PaneInspector;
class CVGBridge;

/// <summary>
/// Inspector representation of a streaming web camera.
/// </summary>
class InspBarVideoStream : public wxWindow
{
public:

	/// <summary>
	/// Cached reference to relevant application functionality.
	/// </summary>
	CVGBridge * bridge;

	/// <summary>
	/// Cached GUID of the equipment the webcam was registered from.
	/// </summary>
	std::string eqGUID;

	/// <summary>
	/// Cached network data for the camera stream's URI.
	/// </summary>
	CamChannel camChan;

	/// <summary>
	/// The label of the camera in the UI.
	/// </summary>
	wxStaticText * staticText;

	/// <summary>
	/// The camara icon.
	/// </summary>
	wxStaticBitmap * icon;

	/// <summary>
	/// The original UI color. Used to reset the color of the UI after a
	/// drag operation.
	/// </summary>
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

	/// <summary>
	/// Contructor.
	/// </summary>
	/// <param name="parent">The UI parent window.</param>
	/// <param name="eqGUID">The equipment GUID of the equipment that registered the webcam.</param>
	/// <param name="bridge">A reference to the app's bridge interface.</param>
	/// <param name="camChan">Network data for the camera stream's URI.</param>
	InspBarVideoStream(wxWindow * parent, const std::string & eqGUID, CVGBridge * bridge, const CamChannel & camChan);

	void OnMotion(wxMouseEvent& evt);
	void OnLeftButtonDown(wxMouseEvent& evt);
	void OnLeftButtonUp(wxMouseEvent& evt);
	void OnMouseCaptureChanged(wxMouseCaptureChangedEvent& evt);
	void OnMouseCaptureLost(wxMouseCaptureLostEvent& evt);

	wxDECLARE_EVENT_TABLE();
};