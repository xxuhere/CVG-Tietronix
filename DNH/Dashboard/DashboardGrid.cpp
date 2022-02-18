#include "DashboardGrid.h"

DashboardGrid::DashGroup::DashGroup(const std::string& eqGUID, const std::string& eqPurpose)
{
	this->eqGUID = eqGUID;
	this->eqPurpose = eqPurpose;
}

Tile* DashboardGrid::DashGroup::Find(const std::string& paramId)
{
	for (Tile* tile : this->tiles)
	{
		if(tile->GetType() != Tile::Type::Param)
			continue;

		TileParam * ele = (TileParam*)tile;
		if (ele->ParamID() == paramId)
			return ele;
	}
	return nullptr;
}

DashboardGrid::DashboardGrid(int gridCellSize, const std::string& name)
{
	this->name = name;
	this->gridCellSize = gridCellSize;
}

DashboardGrid::DashboardGrid(DashboardGrid* deepCopyTarg)
{
	this->gridCellSize = deepCopyTarg->gridCellSize;

	std::map<Tile*, Tile*> origToClone;

	for(Tile* tile : deepCopyTarg->tiles)
	{
		Tile * clone = tile->Clone();
		this->tiles.push_back(clone);
		origToClone[tile] = clone;
	}

	for(auto it : deepCopyTarg->equipmentGrouping)
	{
		DashboardGrid::DashGroup * origGroup = it.second;
		DashGroup * newDash = new DashGroup(it.first, origGroup->EqPurpose());
		this->equipmentGrouping[it.first] = newDash;

		// For every old element, create a group with the new
		// copy in it - thus creating a deep copy of every group.
		const int ct = origGroup->Size();
		for(int i = 0; i < ct; ++i)
			newDash->Add(origToClone[origGroup->GetTile(i)]);
	}
}

DashboardGrid::~DashboardGrid()
{
	for(auto it : this->equipmentGrouping)
	{
		DashGroup * dg = it.second;
		delete dg;
	}
	this->equipmentGrouping.clear();

	for(Tile* tile : this->tiles)
		delete tile;
}


bool DashboardGrid::DashGroup::Forget(const std::string& paramId)
{
	for (
		auto it = this->tiles.begin();
		it != this->tiles.end();
		++it)
	{
		if((*it)->GetType() != Tile::Type::Param)
			continue;

		TileParam * ele = (TileParam*)(*it);
		if (ele->ParamID() == paramId)
		{
			this->tiles.erase(it);
			return true;
		}
	}
	return false;
}

bool DashboardGrid::DashGroup::Add(Tile* tile)
{
	this->tiles.push_back(tile);
	return true;
}

bool DashboardGrid::DashGroup::Destroy(const std::string& paramId)
{
	for (
		auto it = this->tiles.begin();
		it != this->tiles.end();
		++it)
	{
		if((*it)->GetType() != Tile::Type::Param)
			continue;

		TileParam * ele = (TileParam*)*it;

		if (ele->ParamID() == paramId)
		{
			this->tiles.erase(it);
			return true;
		}
	}
	return false;
}

bool DashboardGrid::DashGroup::Destroy(Tile* tile)
{
	for (
		auto it = this->tiles.begin();
		it != this->tiles.end();
		++it)
	{
		if ((*it) == tile)
		{
			// TODO: Refactor
			//(*it)->DestroyUIImpl();
			this->tiles.erase(it);
			return true;
		}
	}
	return false;
}

bool DashboardGrid::DashGroup::Contains(const std::string& paramId)
{
	return this->Find(paramId) != nullptr;
}

void DashboardGrid::DashGroup::Reset(const std::string& eqGUID, const std::string& eqPurpose)
{
	this->eqGUID	= eqGUID;
	this->eqPurpose = eqPurpose;
}

void DashboardGrid::RedoDashLayout()
{
	// TODO:
}

void DashboardGrid::Clear()
{
	// Clear the vector of all items.
	// TODO: Refactor
	//for (DashboardElement* de : elements)
	//	de->DestroyUIImpl();

	this->tiles.clear();

	// Clear the organized directory of all items.
	for (auto it : this->equipmentGrouping)
		delete it.second;

	this->equipmentGrouping.clear();
}

bool DashboardGrid::Remove(
	const std::string& guid, const std::string& paramID)
{
	// Find and remove it from the map first. Because of the
	// maps property and how it leads to then searching in smaller
	// 
	auto itEqFind = this->equipmentGrouping.find(guid);
	if (itEqFind == this->equipmentGrouping.end())
		return false;

	if (!itEqFind->second->Destroy(paramID))
		return false;

	// If we removed the DashGroup's last item, there's no 
	// reason to keep around the empty container.
	if (itEqFind->second->Size() == 0)
		this->equipmentGrouping.erase(itEqFind);

	// If it was removed in the equipmentGrouping, it should also
	// be removable in the elements.
	for (
		auto it = this->tiles.begin(); 
		it != this->tiles.end(); 
		++it)
	{
		if((*it)->GetType() != Tile::Type::Param)
			continue;

		TileParam * ele = (TileParam*)(*it);
		if ((*it)->EqGUID() == guid && ele->ParamID() == paramID)
		{
			// Just erase, it's already been destroyed above.
			this->tiles.erase(it);
			return true;
		}
	}
	return false;
}

bool DashboardGrid::Remove(Tile* tile)
{
	const std::string guid = tile->EqGUID();

	auto itEqFind = this->equipmentGrouping.find(guid);
	if (itEqFind == this->equipmentGrouping.end())
		return false;

	if (!itEqFind->second->Destroy(tile))
		return false;

	// If we removed the DashGroup's last item, there's no 
	// reason to keep around the empty container.
	if (itEqFind->second->Size() == 0)
		this->equipmentGrouping.erase(itEqFind);

	// If it was removed in the equipmentGrouping, it should also
	// be removable in the elements.
	for (
		auto it = this->tiles.begin(); 
		it != this->tiles.end(); 
		++it)
	{

		if ((*it)->EqGUID() == guid && (*it) == tile)
		{
			// Just erase, it's already been destroyed above.
			this->tiles.erase(it);
			return true;
		}
	}
	return false;
}


bool DashboardGrid::Contains(const std::string& guid, const std::string& paramID) const
{
	// The equipmentGrouping and elements have the same
	// content, but the equipmentGrouping naturally lends
	// itself for more efficient seeking.
	auto it = this->equipmentGrouping.find(guid);
	if (it == this->equipmentGrouping.end())
		return false;

	return it->second->Contains(paramID);
}

bool DashboardGrid::ContainsDashboardElement(TileParam* de) const
{
	return this->Contains(de->EqGUID(), de->ParamID());
}

bool DashboardGrid::AreCellsFree(const wxPoint& gridPt, const wxSize& gridSz)
{
	std::set<Tile*> ignoreFiller;
	return this->AreCellsFree(gridPt, gridSz, ignoreFiller);
}

bool DashboardGrid::AreCellsFree(const wxPoint& gridPt, const wxSize& gridSz, Tile* ignore)
{
	std::set<Tile*> ignores;
	ignores.insert(ignore);
	return this->AreCellsFree(gridPt, gridSz, ignores);
}

bool DashboardGrid::AreCellsFree(const wxPoint& gridPt, const wxSize& gridSz, std::set<Tile*> ignores)
{
	const wxPoint end = gridPt + gridSz;
	const wxPoint & start = gridPt; // For consistent naming

	// Do a bound check on each existing element
	for(Tile* ele : this->tiles)
	{
		if(ignores.find(ele) == ignores.end())
			continue;

		const wxPoint& eleStart = ele->gridPos;
		const wxPoint eleEnd = ele->gridPos + ele->gridSize;

		// Do a rejection test for the min and max of both X and Y.
		// If there isn't a rejection test that passes, there's an overlap.
		if(eleEnd.x < start.x || start.x < eleEnd.x)
			continue;
		if(eleEnd.y < start.y || start.y < eleEnd.y)
			continue;
		if(eleStart.x > end.x || end.x > eleStart.x)
			continue;
		if(eleStart.y > end.y || end.y > eleStart.y)
			continue;

		return false;
	}
	return true;
}

bool DashboardGrid::InDocumentBounds(const wxPoint& gridPt, const wxSize& gridSz) const
{
	// rejection tests on all 4 sides.
	if(gridPt.x < 0)
		return false;
	if(gridPt.y < 0)
		return false;
	if(gridPt.x + gridSz.x > this->gridWidth)
		return false;
	if(gridPt.y + gridSz.y > this->gridHeight)
		return false;

	return true;
}

TileParam* DashboardGrid::AddDashboardElement(
	const std::string& eqGuid, 
	const std::string& eqPurpose,
	CVG::ParamSPtr ptr, 
	int gridX,
	int gridY,
	const std::string& uiImplName,
	int gridWidth, 
	int gridHeight,
	bool allowOverlap)
{
	// Sanity check
	if(ptr == nullptr)
		return nullptr;

	// This function does not support creation on overlapping
	if(!allowOverlap && 
		!this->AreCellsFree(
			wxPoint(gridX, gridY), 
			wxSize(gridWidth, gridHeight)))
	{ 
		return nullptr;
	}

	TileParam* ele = 
		new TileParam(
			this, 
			eqGuid,
			ptr);

	// A common reason it could fail is if the equipment GUID is invalid.
	if(ele == nullptr)
		return nullptr;

	// Add to the listing of widgets
	this->tiles.push_back(ele);

	// Add to the listing of widgets grouped by equipment GUID
	DashGroup * grouping;
	auto it = this->equipmentGrouping.find(eqGuid);
	if(it == this->equipmentGrouping.end())
	{
		grouping = new DashGroup(eqGuid, eqPurpose);
		this->equipmentGrouping[eqGuid] = grouping;
	}
	else
		grouping = it->second;

	grouping->Add(ele);

	// Layout
	ele->SetDimensions(
		wxPoint(gridX, gridY), 
		wxSize(gridWidth, gridHeight));

	return ele;
}

TileCam* DashboardGrid::AddDashboardCam(
	const std::string& eqGuid,
	const std::string& eqPurpose,
	int gridX,
	int gridY,
	CamChannel camChan,
	int gridWidth, 
	int gridHeight,
	bool allowOverlap)
{
	// This function does not support creation on overlapping
	if(!allowOverlap && 
		!this->AreCellsFree(
			wxPoint(gridX, gridY), 
			wxSize(gridWidth, gridHeight)))
	{ 
		return nullptr;
	}

	TileCam* cam = 
		new TileCam(
			this, 
			eqGuid,
			camChan);

	// Add to the listing of widgets
	this->tiles.push_back(cam);

	// Add to the listing of widgets grouped by equipment GUID
	DashGroup * grouping;
	auto it = this->equipmentGrouping.find(eqGuid);
	if(it == this->equipmentGrouping.end())
	{
		grouping = new DashGroup(eqGuid, eqPurpose);
		this->equipmentGrouping[eqGuid] = grouping;
	}
	else
		grouping = it->second;

	grouping->Add(cam);

	// Layout
	cam->SetDimensions(
		wxPoint(gridX, gridY), 
		wxSize(gridWidth, gridHeight));

	return cam;
}

wxPoint DashboardGrid::ConvertPixelsToGridCell(const wxPoint& pixel)
{
	return 
		wxPoint(
			pixel.x / this->gridCellSize,
			pixel.y / this->gridCellSize);
}

Tile* DashboardGrid::GetTileAtCell(const wxPoint& cell)
{
	for(Tile* tile : this->tiles)
	{
		if(tile->CellInTile(cell) == true)
			return tile;
	}
	return nullptr;
}

Tile* DashboardGrid::GetTileAtPixel(const wxPoint& pixel)
{
	wxPoint cell = ConvertPixelsToGridCell(pixel);
	return GetTileAtCell(cell);
}

bool DashboardGrid::MoveCell(
	Tile* toMove, 
	const wxPoint& newCellPos, 
	const wxSize& newCellSize, 
	bool collisionCheck)
{
#if _DEBUG
	auto itFind = std::find(this->tiles.begin(), this->tiles.end(), toMove);
	assert(itFind != this->tiles.end());
#endif

	assert(newCellSize.x >= 1 && newCellSize.y >= 1);

	if(!this->InDocumentBounds(newCellPos, newCellSize))
		return false;

	return toMove->SetDimensions(newCellPos, newCellSize, collisionCheck);
}

std::vector<DashboardGrid::GUIDPurposePair> DashboardGrid::GetEquipmentList() const
{
	std::vector<DashboardGrid::GUIDPurposePair> ret;
	for(auto it : this->equipmentGrouping)
	{
		GUIDPurposePair newEntry;
		newEntry.guid		= it.second->EqGUID();
		newEntry.purpose	= it.second->EqPurpose();
		ret.push_back(newEntry);
	}
	return ret;
}

DashboardGrid::RemapRet DashboardGrid::RemapInstance(const std::string& guidOld, const std::string& guidNew, CVGBridge * bridge)
{
	auto itOrig = this->equipmentGrouping.find(guidOld);
	if(itOrig == this->equipmentGrouping.end())
	{
		// Nothing to remap
		return RemapRet::NoChange;
	}

	if(this->equipmentGrouping.find(guidNew) != this->equipmentGrouping.end())
	{
		// Can't remap to something already known about
		return RemapRet::Illegal;
	}

	CVG::BaseEqSPtr eq = bridge->CVGB_GetEquipment(guidNew);
	DashGroup* dashGroupRemap = itOrig->second;

	// Change the old GUID to be replaced with a new GUID
	std::string newPurpose = eq->Purpose();
	dashGroupRemap->Reset(guidNew, newPurpose);
	this->equipmentGrouping.erase(itOrig);
	this->equipmentGrouping[guidNew] = dashGroupRemap;

	// And also update the Param references
	for(int i = 0; i < dashGroupRemap->Size(); ++i)
	{
		Tile* tile = dashGroupRemap->GetTile(i);
		if(tile->GetType() != Tile::Type::Param)
			continue;

		TileParam* ele = (TileParam*)tile;
		ele->_Reset(guidNew, newPurpose, eq->GetParam(ele->ParamID()));
	}

	return RemapRet::Success;
}

void DashboardGrid::RefreshAllParamInstances(CVGBridge* bridge)
{
	for(Tile* tile : this->tiles)
	{
		if(tile->GetType() != Tile::Type::Param)
			continue;

		TileParam* ele = (TileParam*)tile;

		CVG::BaseEqSPtr eq = bridge->CVGB_GetEquipment(tile->EqGUID());
		if(eq == nullptr)
			continue;

		ele->_Reset(eq->GUID(), eq->Purpose(), eq->GetParam(ele->ParamID()));
	}
}

bool DashboardGrid::SetCellSize(const wxSize& sz)
{
	// NOTE: There's probably more that can be done here, especially 
	// if there are elements out of bounds after the resize. 
	//
	// For now we're going to ignore that issue because of higher 
	// priority issues. (wleu 11/29/2021)
	this->gridWidth = sz.x;
	this->gridHeight = sz.y;
	return true;
}