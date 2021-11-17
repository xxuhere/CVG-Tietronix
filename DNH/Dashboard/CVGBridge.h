#pragma once
#include <string>
#include <Params/Param.h>

/// <summary>
/// An interface class for InspectorParam elements to send drag-and-drop and 
/// Param edit notifications.
/// </summary>
class CVGBridge
{
public:

	//		PARAM CACHE FUNCTIONS
	//////////////////////////////////////////////////

	/// <summary>
	/// Sets the value of a cached parameter and sends a server request to 
	/// match the value server-side.
	/// </summary>
	/// <param name="eq">The equipment GUID that owns the Param.</param>
	/// <param name="param">The Param ID.</param>
	/// <param name="value">The string equivalent of the value to set.</param>
	virtual void CVGB_SetValue(const std::string& eq, const std::string& param, const std::string& value) = 0;

	/// <summary>
	/// Get the value of a cached parameter.
	/// 
	/// Only retrieving a value as a string is supported. But a string value
	/// is expected to have universal support for all Param types.
	/// </summary>
	/// <param name="eq">The equipment GUID that owns the Param.</param>
	/// <param name="param">The Param ID.</param>
	/// <param name="outVal">The Param value as a string.</param>
	/// <returns>
	/// True if the value was retrieved. Else, the Param could not be found, and the
	/// contents of outVal should be ignored.
	/// </returns>
	virtual bool CVGB_GetValue(const std::string& eq, const std::string& param, std::string& outVal) = 0;

	//		PARAMINSPECTOR CALLS
	//////////////////////////////////////////////////

	/// <summary>
	/// Expected to be called by an InsWidgetParam implementation.
	/// 
	/// Called at the start of a drag operation to allow inspector entries to be
	/// dragged into the dashboard.
	/// </summary>
	/// <param name="eq">The Equipment GUID that owned the Param being dragged.</param>
	/// <param name="p">The Param being dragged.</param>
	virtual void Param_OnDragStart(const std::string& eq, CVG::ParamSPtr p) = 0;

	/// <summary>
	/// Expected to be called by an InsWidgetParam implementation.
	/// 
	/// Called at the end of a drag operation to finalize an inspector entry being
	/// dragged into the dashboard.
	/// </summary>
	/// <param name="eq">The Equipment GUID that owned the Param being dragged.</param>
	/// <param name="p">The Param being dragged.</param>
	virtual void Param_OnDragEnd(const std::string& eq, CVG::ParamSPtr p) = 0;

	/// <summary>
	/// Expected to be called by an InsWidgetParam implementation.
	/// 
	/// Called to notify the bridge that the current drag session has been aborted.
	/// </summary>
	virtual void Param_OnDragCancel() = 0;

	/// <summary>
	/// Expected to be called by an InsWidgetParam implementation.
	/// 
	/// Called to notify the brige that the the mouse has been moved during the drag
	/// session. The UI behind the bridge should update to reflect the new dragged
	/// position.
	/// </summary>
	/// <param name="eq">The Equipment GUID that owned the Param being dragged.</param>
	/// <param name="p">The Param being dragged.</param>
	virtual void Param_OnDragMotion(const std::string& eq, CVG::ParamSPtr p) = 0;
};