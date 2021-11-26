#include "DashboardElement.h"
#include "DashElements/IDashEle.h"
#include "DashboardGrid.h"

DashboardElement::DashboardElement(
	DashboardGrid* owner,
	const std::string& eqGuid, 
	CVG::ParamSPtr param)
{
	this->gridOwner = owner;

	// TODO: Reimplement
	//CVG::BaseEqSPtr eqPtr = this->bridge->CVGB_GetEquipment(eqGuid);
	this->guid = eqGuid;
	this->param = param;
	this->paramID = param->GetID();

	// Cache the label. This allow us to show what used to be there if 
	// the Param is disconnected - either because the equipment was 
	// disconnected or the client was disconnected from the server.
	this->label = param->GetLabel();
	if(this->label.empty())
		this->label = param->GetID();
}

void DashboardElement::_Reset(const std::string& guid, const std::string& purpose, CVG::ParamSPtr ptr )
{
	this->guid = guid;
	this->purpose = purpose;

	// For right now it's assumed if we reset, the old param is no long valid. 
	// 
	// Another option to _Reset is to make a placeholder Param, but if a replacement
	// isn't specified, just leaving the old one alone results in the same outcome.
	if(ptr != nullptr)
	{
		this->param = ptr;
	}
}

bool DashboardElement::SetDimensions(const wxPoint& pt, const wxSize& sz, bool checkCollisions)
{
	if(checkCollisions == true)
	{ 
		if(!this->gridOwner->AreCellsFree(pt, sz, this))
			return false;
	}

	int cellDim			= this->gridOwner->GridCellSize();

	this->gridPos		= pt;
	this->gridSize		= sz;
	this->cachedPos		= pt * cellDim;
	this->cachedSize	= wxSize(sz.x * cellDim, sz.y * cellDim);

	const int resrvNameHgt = 20;
	const int bottomPad = 5;

	// The uiPixel dimensions are within the pixel boundaries
	this->uiPixelPos = this->cachedPos;
	this->uiPixelSize = this->cachedSize;
	// Apply name and border padding
	static const int HBORDER = 2;
	this->uiPixelPos.x += HBORDER;
	this->uiPixelSize.x -= HBORDER * 2;
	this->uiPixelPos.y += resrvNameHgt;
	this->uiPixelSize.y -= (resrvNameHgt + bottomPad);

	return true;
}

bool DashboardElement::SwitchParam(std::string& eq, CVG::ParamSPtr param, bool resetDefault)
{
	//TODO:
	return false;
}



bool DashboardElement::CellInElement(const wxPoint& cell)
{
	if(cell.x < this->gridPos.x)
		return false;

	if(cell.y < this->gridPos.y)
		return false;

	if(cell.x >= this->gridPos.x + this->gridSize.x)
		return false;

	if(cell.y >= this->gridPos.y + this->gridSize.y)
		return false;

	return true;
}

