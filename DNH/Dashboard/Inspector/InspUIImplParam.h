#pragma once
#include <wx/wx.h>
#include "../CVGBridge.h"

class InspBarParam;

/// <summary>
/// Inspector widget parameter. An implementation of a UI to
/// control a Param
/// </summary>
class InspUIImplParam
{
protected:
	/// <summary>
	/// The InspBarParam that the InspUIImplParam is contained in.
	/// </summary>
	InspBarParam * parent;

	/// <summary>
	/// Reference to the CVGBridge interface to notify the application
	/// when a value should be changed, or to retrieved the current
	/// cached value.
	/// </summary>
	CVGBridge* bridge;

protected:

	/// <summary>
	/// Constructor.
	/// </summary>
	/// <param name="parent">The InspBarParam containing the InspUIImplParam.</param>
	/// <param name="bridge">A reference to the CVGbridge.</param>
	InspUIImplParam(InspBarParam * parent, CVGBridge* bridge);

	/// <summary>
	/// Send a request to the server to change the Param's value.
	/// </summary>
	/// <param name="valAsString">The new value of the Param as a string.</param>
	void SendParamValueToServer(const std::string & valAsString);

public:

	/// <summary>
	/// Update the display of the variable's value to match the
	/// cached value.
	/// </summary>
	virtual void UpdateDisplay() = 0;

	/// <summary>
	/// Destroy the InspUIImplParam's UI (this is assumed to be the
	/// same UI that would be retrieved from GetWindow()).
	/// </summary>
	virtual void DestroyUI() = 0;

	/// <summary>
	/// Get a reference to the UI object.
	/// </summary>
	/// <returns>The wxWindow implementing the InspUIImplParam.</returns>
	virtual wxWindow* GetWindow() = 0;

	virtual ~InspUIImplParam();
};