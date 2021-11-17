#pragma once
#include <wx/wx.h>
#include <Params/Param.h>
#include <Equipment.h>

class InspectorParam;
class PaneInspector;
class CVGBridge;

/// <summary>
/// A UI element in the PaneInspector that contains the listing of all
/// the Params known (the params are shown as InspectorParams).
/// </summary>
class InspectorEquipment : public wxWindow
{
public:

	/// <summary>
	/// UI IDs.
	/// </summary>
	enum CmdIDs
	{
		/// <summary>
		/// The expand/compress button.
		/// </summary>
		ExpandCompress
	};
public:

	/// <summary>
	/// The cached Equipment being represented by the InspectorEquipment.
	/// </summary>
	CVG::BaseEqSPtr equipment;

	/// <summary>
	/// Reference to the inspector that contains the InspectorEquipment.
	/// </summary>
	PaneInspector* owner;

	/// <summary>
	/// The contained ParamUIs.
	/// </summary>
	std::map<std::string, InspectorParam*> paramUIs;

	/// <summary>
	/// The text showing the Equipment's label as a titlebar.
	/// </summary>
	wxStaticText* titleText		= nullptr;

	/// <summary>
	/// The sizer maintaining the InspectorEquipment.
	/// </summary>
	wxBoxSizer * outerSizer		= nullptr;

	/// <summary>
	/// The window containing the InspectorParams.
	/// </summary>
	wxWindow* paramWinContainer = nullptr;

	/// <summary>
	/// The sizer for the contained InspectorParams.
	/// </summary>
	wxBoxSizer * paramSizer		= nullptr;

	/// <summary>
	/// The button to expand/compress the contents of the InspectorEquipment.
	/// </summary>
	wxButton* btnExpandCompress;

	/// <summary>
	/// If true, the equipment is in expanded mode.
	/// If false, the equipment is in compressed mode.
	/// </summary>
	bool expanded = true;

public:

	/// <summary>
	/// Constructor.
	/// </summary>
	/// <param name="owner">The PaneInspector to be parent to.</param>
	/// <param name="bridge">The bridge to interface with for certain events and utilities.</param>
	/// <param name="eq">The cached Equipment the InspectorEquipment is representing.</param>
	InspectorEquipment(PaneInspector * owner, CVGBridge* bridge, CVG::BaseEqSPtr eq);

	/// <summary>
	/// Clear references to smart pointers or things that won't be destroyed
	/// or dereferenced when the UI is destroyed and could cause memory leaks.
	/// </summary>
	void ClearReferences();

	/// <summary>
	/// Set the color of the InspectorEquipment if it represents this CVGDashboard.
	/// </summary>
	void SetBGColorAsSelf();

	/// <summary>
	/// Set the color of all InspectorEquipment that do not represent this CVGDashboard.
	/// </summary>
	void SetBGColorAsOther();

	/// <summary>
	/// The callback for the button to toggle between compressed and expanded.
	/// </summary>
	void OnButtonExpandCompress(wxCommandEvent& evt);

	DECLARE_EVENT_TABLE();
};