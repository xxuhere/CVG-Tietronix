#include "DashboardTile.h"

DashboardTile::DashboardTile(DashboardGrid * gridOwner, const std::string& eqGuid)
{
	this->gridOwner = gridOwner;
	this->guid = eqGuid;
}

void DashboardTile::SetLabel(const std::string& label)
{
	this->label = label;
}

bool DashboardTile::CellInTile(const wxPoint& cell)
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


DashboardTile::~DashboardTile()
{}