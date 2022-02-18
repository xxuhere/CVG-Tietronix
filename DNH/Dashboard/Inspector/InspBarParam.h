#pragma once
#include "Params/Param.h"
#include <wx/wx.h>
#include <string>

class CVGBridge;
class InspUIImplParam;
class PaneInspector;

/// <summary>
/// The UI representation for a Param in the PaneInspector.
/// </summary>
class InspBarParam : public wxWindow
{
public:
	/// <summary>
	/// IDs for UI and commands.
	/// </summary>
	enum MenuID
	{
		/// <summary>
		/// The option for the "Hide" menu option.
		/// </summary>
		Toggle
	};

	/// <summary>
	/// The sizer for the InspBarParam.
	/// </summary>
	wxBoxSizer * sizer = nullptr;

	/// <summary>
	/// The text label for the label.
	/// </summary>
	wxStaticText * nameText = nullptr;

	/// <summary>
	/// The widget implementing representation and control
	/// of the Param.
	/// </summary>
	InspUIImplParam * widgetImpl = nullptr;

	/// <summary>
	/// The PanelInspector the Param is being shown in.
	/// </summary>
	PaneInspector* owner = nullptr;

	/// <summary>
	/// Reference to the CVGBridge to notify the application of 
	/// special events, and well as accessing application utilities
	/// (without exposing the entire application).
	/// </summary>
	CVGBridge* bridge;

	/// <summary>
	/// The Param's Equipment's GUID.
	/// </summary>
	std::string eqGUID;

	/// <summary>
	/// The cache's Param value.
	/// </summary>
	CVG::ParamSPtr param;

	/// <summary>
	/// A cached value of the InspBarParam's hidden or visible state.
	/// </summary>
	bool visible = true;


	wxColour origBGColor;

public:
	/// <summary>
	/// Constructor.
	/// </summary>
	/// <param name="parent">Parent for the InspBarParam's inherited wxWindow.</param>
	/// <param name="owner">The PaneInspector that contains the InspBarParam.</param>
	/// <param name="bridge">A reference to the CVGBridge interface.</param>
	/// <param name="eqGUID">The Param's Equipment's GUID.</param>
	/// <param name="param">A reference to the Param in the application's cache.</param>
	InspBarParam(wxWindow * parent, PaneInspector* owner, CVGBridge* bridge, std::string eqGUID, CVG::ParamSPtr param);

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

	/// <summary>
	/// Set the widget implementation
	/// </summary>
	void SetDefaultWidget();

	/// <summary>
	/// Rebuild the sizer. 
	/// 
	/// Called when widgetImpl is recreated, so the sizer needs to be
	/// recreated to accomodate it.
	/// </summary>
	void RebuildSizer();

	/// <summary>
	/// Pull the cached Param value and update the UI to match it.
	/// </summary>
	void UpdateWidgetValue();

	/// <summary>
	/// Clear references to smart pointers or things that won't be destroyed
	/// when the UI is destroyed and could cause memory leaks.
	/// </summary>
	void ClearReferences();

	/// <summary>
	/// Right click callback.
	/// 
	/// Creates a drop-down menu.
	/// </summary>
	void OnRightMouseDown(wxMouseEvent& evt);

	/// <summary>
	/// Menu callback for the "Hide" option.
	/// </summary>
	void OnMenuToggle(wxCommandEvent& evt);

	// Mouse events for handling dragging on to the canvas
	void OnMotion(wxMouseEvent& evt);
	void OnLeftButtonDown(wxMouseEvent& evt);
	void OnLeftButtonUp(wxMouseEvent& evt);
	void OnMouseCaptureChanged(wxMouseCaptureChangedEvent& evt);
	void OnMouseCaptureLost(wxMouseCaptureLostEvent& evt);

	wxDECLARE_EVENT_TABLE();
};