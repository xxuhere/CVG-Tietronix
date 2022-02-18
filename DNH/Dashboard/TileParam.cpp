#include "TileParam.h"
#include "DashElements/DashParamUIImpl.h"
#include "DashboardGrid.h"

TileParam::TileParam(
	DashboardGrid* owner,
	const std::string& eqGuid, 
	CVG::ParamSPtr param)
	: Tile(owner, eqGuid)
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

void TileParam::_Reset(const std::string& guid, const std::string& purpose, CVG::ParamSPtr ptr )
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

bool TileParam::SetDimensions(const wxPoint& pt, const wxSize& sz, bool checkCollisions)
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

void TileParam::SetLabel(const std::string& newLabel)
{
	Tile::SetLabel(newLabel);

	// If the label is empty, use fallbacks based on parameter values.
	if(this->label.empty())
		this->label = this->DefaultLabel();
}

bool TileParam::SwitchParam(std::string& eq, CVG::ParamSPtr param, bool resetDefault)
{
	//TODO:
	return false;
}

std::string TileParam::DefaultLabel() const
{
	std::string lab = this->param->GetLabel();
	if(!lab.empty())
		return lab;

	return this->param->GetID();
}

Tile::Type TileParam::GetType()
{
	return Type::Param;
}

Tile* TileParam::Clone()
{
	return new TileParam(*this);
}