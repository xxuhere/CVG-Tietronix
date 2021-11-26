#pragma once
#include <string>
#include <wx/wx.h>
#include <boost/optional.hpp>

#include <Params/Param.h>

class DashboardGrid;
class CVGBridge;

/// <summary>
/// Contains an items on the dashboard. 
/// 
/// NOTE THAT THIS IS NOT THE ACTUAL UI ELEMENT ON THE DASHBOARD,
/// IT IS ONLY A CONTAINER. (See usage of uiImpl for more information
/// on the actual UI implementations).
/// </summary>
class DashboardElement
{
	friend DashboardGrid;

protected:
	// It may appear that there's quite a bit of redundant information
	// being stored, as far as holding the 

	/// <summary>
	/// The equipment ID of the parameter.
	/// </summary>
	std::string guid;

	CVG::ParamSPtr param;

	/// <summary>
	/// The position on the dashboard, in grid cell units.
	/// </summary>
	wxPoint gridPos;

	/// <summary>
	/// The size of the 
	/// </summary>
	wxSize gridSize;

	/// <summary>
	/// The position of the element on the canvas, in pixels.
	/// </summary>
	wxPoint cachedPos;

	/// <summary>
	/// The size of the element on the canvas, in pixels.
	/// </summary>
	wxSize cachedSize;

	wxPoint uiPixelPos;

	wxSize uiPixelSize;

	/// <summary>
	/// The cached equipment of the parameter, when connecting an existing
	/// element to a new connection.
	/// </summary>
	std::string purpose;

	/// <summary>
	/// The name of the param last connected to.
	/// </summary>
	std::string paramID;

	/// <summary>
	/// The label currently or last used. By default this will be the 
	/// Param's label, but may be changed.
	/// </summary>
	std::string label;

	/// <summary>
	/// The name of the last valid uiImplementation name assigned.
	/// </summary>
	std::string uiImplName;

	/// <summary>
	/// Reference to grid.
	/// </summary>
	DashboardGrid* gridOwner = nullptr;

private:
	// Should only be called by DashboardGrid GUID replacement function.
	void _Reset(const std::string& guid, const std::string& purpose, CVG::ParamSPtr ptr );

public:

	DashboardElement(
		DashboardGrid* owner,  
		const std::string& eqGuid, 
		CVG::ParamSPtr param);

	/// <summary>
	/// Change the UI implementation of the cell based on an 
	/// implementation name.
	/// </summary>
	/// <param name="implName">The name to switch to.</param>
	/// <returns>
	/// True if successful. Else, the implementation name was not recognized.
	/// </returns>
	bool SwitchUIImplementation(const std::string& implName);

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	bool SwitchUIDefault();

	bool SetDimensions(const wxPoint& pt, const wxSize& sz, bool checkCollisions = true);

	inline wxPoint CellPos() const
	{ return this->gridPos; }

	inline wxSize CellSize() const
	{ return this->gridSize; }

	inline wxPoint PixelPos() const
	{ return this->cachedPos; }

	inline wxSize PixelSize() const
	{ return this->cachedSize; }

	inline wxPoint UIPos() const
	{ return this->uiPixelPos; }

	inline wxSize UISize() const
	{ return this->uiPixelSize; }

	inline wxPoint CachedSize() const;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="eq"></param>
	/// <param name="param"></param>
	/// <param name="resetDefault"></param>
	/// <returns></returns>
	bool SwitchParam(std::string& eq, CVG::ParamSPtr param, bool resetDefault = true);

	inline const std::string& EqGUID() const
	{ return this->guid; }

	inline const std::string& ParamID() const
	{ return this->paramID; }

	inline const std::string& EqPurpose() const
	{return this->purpose; }

	inline std::string Label() const
	{ return this->label; }

	bool CellInElement(const wxPoint& cell);

	inline CVG::ParamSPtr Param()
	{ return this->param; }

	void SetUIImplName(const std::string& str)
	{ this->uiImplName = str;}

	std::string GetUIImplName() const
	{ return this->uiImplName; }
};