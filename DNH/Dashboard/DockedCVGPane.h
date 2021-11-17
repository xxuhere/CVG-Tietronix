#pragma once

#include <wx/wx.h>
#include <string>
#include "nlohmann/json.hpp"
#include "Equipment.h"

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
	/// The title of the window.
	/// Used as the string for the docked pane titlebar.
	/// </summary>
	/// <returns>The pane's titlebar text.</returns>
	virtual std::string Title() = 0;

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

	/// <summary>
	/// Called when a cached parameter is changed.
	/// </summary>
	/// <param name="eq">The Equipment the Param belongs to.</param>
	/// <param name="param">The Param that was changed.</param>
	virtual void _CVG_EVT_OnParamChange(CVG::BaseEqSPtr eq, CVG::ParamSPtr param) = 0;
};