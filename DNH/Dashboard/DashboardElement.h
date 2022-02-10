#pragma once
#include <string>

#include <boost/optional.hpp>

#include <Params/Param.h>
#include "DashboardTile.h"

class DashboardGrid;
class CVGBridge;

/// <summary>
/// Contains an items on the dashboard. 
/// 
/// NOTE THAT THIS IS NOT THE ACTUAL UI ELEMENT ON THE DASHBOARD,
/// IT IS ONLY A CONTAINER. (See usage of uiImpl for more information
/// on the actual UI implementations).
/// </summary>
class DashboardElement : public DashboardTile
{
	friend class DashboardGrid;

protected:
	// It may appear that there's quite a bit of redundant information
	// being stored, as far as holding the 

	CVG::ParamSPtr param;

	/// <summary>
	/// The name of the param last connected to.
	/// </summary>
	std::string paramID;

	/// <summary>
	/// The name of the last valid uiImplementation name assigned.
	/// </summary>
	std::string uiImplName;

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

	void SetLabel(const std::string& label);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="eq"></param>
	/// <param name="param"></param>
	/// <param name="resetDefault"></param>
	/// <returns></returns>
	bool SwitchParam(std::string& eq, CVG::ParamSPtr param, bool resetDefault = true);

	inline const std::string& ParamID() const
	{ return this->paramID; }

	inline CVG::ParamSPtr Param()
	{ return this->param; }

	void SetUIImplName(const std::string& str)
	{ this->uiImplName = str;}

	std::string GetUIImplName() const
	{ return this->uiImplName; }

	std::string DefaultLabel() const;

	Type GetType() override;

	DashboardTile* Clone() override;
};