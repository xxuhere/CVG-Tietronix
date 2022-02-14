#pragma once

#include <wx/wx.h>
#include <string>
#include "nlohmann/json.hpp"
#include "Equipment.h"

class DashboardGrid;

using json = nlohmann::json;

/// <summary>
/// A class for a window listening in on the application's events and
/// docked to the RootWindow.
/// 
/// This is expected to be derived off a wxWindow or wxWindow subclass, but
/// that is unknown, so it needs to be multiple-inherited with the wxWindow.
/// </summary>
class DockedCVGPane
{
public:
	/// <summary>
	/// The different dockable pane types.
	/// </summary>
	enum class PaneType
	{
		Dashboard,
		Inspector,
		Log,
		Misc
	};

public:
	/// <summary>
	/// The title of the window.
	/// Used as the string for the docked pane titlebar.
	/// </summary>
	/// <returns>The pane's titlebar text.</returns>
	virtual std::string Title() = 0;

	/// <summary>
	/// Get the type of pane.
	/// </summary>
	/// <returns>The pane type. If not a type that there's a specific
	/// enum for, PaneType.Misc will be returned.</returns>
	virtual PaneType GetPaneType() = 0;

	/// <summary>
	/// The DockedSVGPane's window. This will almost always be a simple 
	/// "return this;"
	/// </summary>
	/// <returns>The wxWindow representation of the pane.</returns>
	virtual wxWindow * _CVGWindow() = 0;

	/// <summary>
	/// Called when the application receives a message from the hub.
	/// </summary>
	/// <param name="msg"></param>
	virtual void _CVG_OnMessage(const std::string & msg) = 0;

	/// <summary>
	/// Called when the application successfully parses and validates
	/// JSON from a hub string.
	/// </summary>
	/// <param name="js"></param>
	virtual void _CVG_OnJSON(const json & js) = 0;

	/// <summary>
	/// Called when the system connects to the hub.
	/// </summary>
	virtual void _CVG_EVT_OnConnect() = 0;

	/// <summary>
	/// Called when the system disconnects to the hub.
	/// </summary>
	virtual void _CVG_EVT_OnDisconnect() = 0;

	/// <summary>
	/// Called when the system caches information for a new equipment
	/// connected to the hub.
	/// </summary>
	/// <param name="eq"></param>
	virtual void _CVG_EVT_OnNewEquipment(CVG::BaseEqSPtr eq) = 0;

	/// <summary>
	/// Called when the system removed a cached equipment because it
	/// was disconnected from the hub.
	/// </summary>
	/// <param name="eq">The cached equipment (about to be erased).</param>
	virtual void _CVG_EVT_OnRemEquipment(CVG::BaseEqSPtr eq) = 0;

	virtual void _CVG_EVT_OnRemapEquipmentPurpose(
		const std::string& purpose, 
		const std::string& prevGUID, 
		const std::string& newGUID );

	/// <summary>
	/// Called when a cached parameter is changed.
	/// </summary>
	/// <param name="eq">The Equipment the Param belongs to.</param>
	/// <param name="param">The Param that was changed.</param>
	virtual void _CVG_EVT_OnParamChange(CVG::BaseEqSPtr eq, CVG::ParamSPtr param) = 0;

	/// <summary>
	/// Called when a new dashboard is added to the app session.
	/// </summary>
	/// <param name="addedGrid">The newly added dashboard.</param>
	virtual void _CVG_Dash_NewBoard(DashboardGrid * addedGrid) = 0;

	/// <summary>
	/// Called when a dashboard is deleted from the app session.
	/// </summary>
	/// <param name="remGrid">The removed dashboard.</param>
	virtual void _CVG_Dash_DeleteBoard(DashboardGrid * remGrid) = 0;

	virtual void _CVG_Session_SavePre();
	virtual void _CVG_Session_SavePost();
	virtual void _CVG_Session_OpenPre(bool append);
	virtual void _CVG_Session_OpenPost(bool append);
	virtual void _CVG_Session_ClearPre();
	virtual void _CVG_Session_ClearPost();
};