#include "DashboardGridInst.h"
#include "DashboardElementInst.h"
#include "DashboardCamInst.h"
#include "DashboardCamInstUI.h"
#include "DashElements/DashParamUIImpl.h"

DashboardGridInst::iterator::iterator(
	std::map<TileParam*, DashboardElementInst*>* pInstMap,
	std::map<TileParam*, DashboardElementInst*>::iterator itInstMap,
	std::map<TileCam*, DashboardCamInst*>* pCamMap,
	std::map<TileCam*, DashboardCamInst*>::iterator itCamMap)
{
	this->pInstMap	= pInstMap;
	this->itInstMap = itInstMap;
	this->pCamMap	= pCamMap;
	this->itCamMap	= itCamMap;
}

DashboardGridInst::iterator::iterator(const DashboardGridInst::iterator& it)
{
	this->pInstMap	= it.pInstMap;
	this->itInstMap = it.itInstMap;
	this->pCamMap	= it.pCamMap;
	this->itCamMap	= it.itCamMap;

}

DashboardGridInst::iterator::~iterator()
{}

DashboardGridInst::iterator& DashboardGridInst::iterator::operator=(const DashboardGridInst::iterator& it)
{
	this->pInstMap	= it.pInstMap;
	this->itInstMap = it.itInstMap;
	this->pCamMap	= it.pCamMap;
	this->itCamMap	= it.itCamMap;

	return *this;
}


DashboardGridInst::iterator* DashboardGridInst::iterator::operator++()
{
	if(this->itInstMap != pInstMap->end())
	{
		++this->itInstMap;

		if(this->itInstMap == pInstMap->end())
		{
			// If we reach the end, transfer to iterating through itCamMap.
			this->itCamMap = this->pCamMap->begin();
		}

	}
	else if(this->itCamMap != pCamMap->end())
		++this->itCamMap;

	return this;
}

std::pair<Tile*, DashboardInst*> DashboardGridInst::iterator::operator*() const
{
	if(this->itInstMap != pInstMap->end())
		return std::pair<Tile*, DashboardInst*>(this->itInstMap->first, this->itInstMap->second);
	
	return std::pair<Tile*, DashboardInst*>(this->itCamMap->first, this->itCamMap->second);
}

bool DashboardGridInst::iterator::operator == (const DashboardGridInst::iterator& it) const
{
	return 
		this->pInstMap	== it.pInstMap	&&
		this->itInstMap == it.itInstMap &&
		this->pCamMap	== it.pCamMap	&&
		this->itCamMap	== it.itCamMap;
}

bool DashboardGridInst::iterator::operator != (const DashboardGridInst::iterator& it) const
{
	return 
		this->pInstMap	!= it.pInstMap	||
		this->itInstMap != it.itInstMap ||
		this->pCamMap	!= it.pCamMap	||
		this->itCamMap	!= it.itCamMap;
}

DashboardGridInst::DashboardGridInst(
	wxWindow* gridCanvasWin,
	CVGBridge* bridge, 
	DashboardGrid* attachedGrid)
{
	this->gridWindow = gridCanvasWin;
	this->bridge = bridge;
	this->grid = attachedGrid;

	for(Tile* tile : *attachedGrid)
	{
		if(tile->GetType() == Tile::Type::Param)
		{ 
			TileParam* ele = (TileParam*)tile;

			DashboardElementInst* eleWrapper = 
				new DashboardElementInst(this, this->bridge, ele);

			this->instMapping[ele] = eleWrapper;

			if(!eleWrapper->SwitchUIImplementation(ele->GetUIImplName()))
			{ 
				eleWrapper->SwitchUIDefault();
			}
		}
		else if(tile->GetType() == Tile::Type::Cam)
		{
			TileCam* cam = (TileCam*)tile;
			DashboardCamInst* camInst = this->Implement(cam);
		}
	}
}

DashboardGridInst::~DashboardGridInst()
{
	for(auto it : this->instMapping)
	{
		DashboardElementInst * eleInst = it.second;

		eleInst->DestroyUIImpl();
		delete eleInst;
	}

	for(auto it : this->camMapping)
	{
		DashboardCamInst * camInst = it.second;

		camInst->DestroyUIImpl();
		delete camInst;
	}

	instMapping.clear();
}

DashboardElementInst* DashboardGridInst::AddDashboardElement(
	int cellX, 
	int cellY, 
	int cellWidth, 
	int cellHeight, 
	const std::string& eqGUID, 
	CVG::ParamSPtr param,
	const std::string& uiImpl)
{
	CVG::BaseEqSPtr eqPtr = this->bridge->CVGB_GetEquipment(eqGUID);
	if(eqPtr == nullptr)
		return nullptr;

	TileParam * tileParam = 
		this->grid->AddDashboardElement(
			eqGUID,
			eqPtr->Purpose(),
			param,
			cellX, 
			cellY,
			uiImpl,
			cellWidth, 
			cellHeight);

	if(tileParam == nullptr)
		return nullptr;

	return this->Implement(tileParam);
}

TileCam* DashboardGridInst::AddDashboardCam(
	int cellX, 
	int cellY, 
	int cellWidth, 
	int cellHeight, 
	const std::string& eqGUID, 
	CamChannel camChan)
{
	CVG::BaseEqSPtr eqPtr = this->bridge->CVGB_GetEquipment(eqGUID);
	if(eqPtr == nullptr)
		return nullptr;

	TileCam * dashCam = 
		this->grid->AddDashboardCam(
			eqGUID,
			eqPtr->Purpose(),
			cellX, 
			cellY,
			camChan,
			cellWidth, 
			cellHeight);

	if(dashCam == nullptr)
		return nullptr;

	return dashCam;
}

bool DashboardGridInst::HasImpl(TileParam * ele)
{
	auto it = this->instMapping.find(ele);
	return it != this->instMapping.end();
}

DashboardElementInst* DashboardGridInst::Implement(TileParam* ele)
{
	auto it = this->instMapping.find(ele);
	if( it != this->instMapping.end() )
		return it->second;

	DashboardElementInst* eleWrapper = 
		new DashboardElementInst(this, this->bridge, ele);

	this->instMapping[ele] = eleWrapper;

	// Create a specific type of dashboard UI
	//
	bool defImpl = true; // Use the default for the Param type?
	std::string uiImpl = ele->GetUIImplName();
	if(!uiImpl.empty())
		defImpl = !eleWrapper->SwitchUIImplementation(uiImpl);

	if(defImpl)
		eleWrapper->SwitchUIDefault();

	eleWrapper->LayoutUIImpl();

	return eleWrapper;
}

DashboardCamInst* DashboardGridInst::Implement(TileCam* cam)
{
	auto it = this->camMapping.find(cam);
	if(it != this->camMapping.end() )
		return it->second;

	DashboardCamInst* camWrapper = 
		new DashboardCamInst(this, this->bridge, cam);

	this->camMapping[cam] = camWrapper;

	camWrapper->Initialize();
	camWrapper->LayoutUIImpl();

	camWrapper->SetURI(cam->URI());
	
	return camWrapper;
}

bool DashboardGridInst::UpdateParamValue(const std::string& eqGuid, const std::string& paramId)
{
	auto itFind = this->grid->equipmentGrouping.find(eqGuid);
	if(itFind == this->grid->equipmentGrouping.end())
		return false;

	DashboardGrid::DashGroup* grp = itFind->second;
	const int ct = grp->Size();
	for(int i = 0; i < ct; ++i)
	{
		Tile* tile = grp->GetTile(i);
		if(tile->GetType() != Tile::Type::Param)
			continue;

		TileParam* ele = (TileParam*)tile;
		auto itFindInst = this->instMapping.find(ele);
		if(itFindInst == this->instMapping.end())
			continue;

		if(ele->Param()->GetID() != paramId)
			continue;

		DashboardElementInst * eleInst = itFindInst->second;
		eleInst->UpdateUI();
	}
	return true;
}

bool DashboardGridInst::RemoveTile(Tile * tile, bool deleteTile)
{
	if(tile->GetType() == Tile::Type::Param)
	{
		TileParam * ele = (TileParam*)tile;

		auto it = this->instMapping.find(ele);
		if(it != this->instMapping.end())
		{
			DashboardElementInst* inst = it->second;
			inst->DestroyUIImpl();
			this->instMapping.erase(it);

			if(deleteTile == false)
				return true;
		}
	}

	return this->grid->Remove(tile);
}

bool DashboardGridInst::MoveCell(Tile* tile, const wxPoint& pos, const wxSize& size)
{
	if( !this->grid->MoveCell(tile, pos, size))
		return false;

	// The way we if-else to detect typing to cast to the correct overloaded,
	// only to eventually deffer to the tile version of MatchEleInstLayout
	// is admittedly clunky. There's probably a more elegant way to do this.

	if (tile->GetType() == Tile::Type::Param)
	{
		return this->MatchEleInstLayout((TileParam*)tile);
	}
	else if(tile->GetType() == Tile::Type::Cam)
	{
		return this->MatchEleInstLayout((TileCam*)tile);
	}

	return false;
}

DashboardGridInst::iterator DashboardGridInst::begin()
{ 
	return iterator(
		&this->instMapping, 
		this->instMapping.begin(), 
		&this->camMapping, 
		this->camMapping.begin());
}

// Allow ranged-based for loops
DashboardGridInst::iterator DashboardGridInst::end()
{ 
	return iterator(
		&this->instMapping, 
		this->instMapping.end(), 
		&this->camMapping, 
		this->camMapping.end());
}

void DashboardGridInst::MatchEleInstLayouts()
{
	for(auto it : this->instMapping)
	{
		DashboardElementInst * eleInst = it.second;
		eleInst->LayoutUIImpl();
	}
	for(auto it : this->camMapping)
	{
		DashboardCamInst * camInst = it.second;
		camInst->LayoutUIImpl();
	}
}

bool DashboardGridInst::MatchEleInstLayout(TileParam* ele)
{
	auto it = this->instMapping.find(ele);
	if(it == this->instMapping.end())
		return false;

	DashboardInst* tileInst = it->second;
	return this->MatchEleInstLayout(tileInst);
}

bool DashboardGridInst::MatchEleInstLayout(TileCam* cam)
{
	auto it = this->camMapping.find(cam);
	if(it == this->camMapping.end())
		return false;

	DashboardInst* tileInst = it->second;
	return this->MatchEleInstLayout(tileInst);
}

bool DashboardGridInst::MatchEleInstLayout(DashboardInst* tileInst)
{
	tileInst->LayoutUIImpl();
	return true;
}

void DashboardGridInst::RefreshInstances()
{
	for(auto it : this->instMapping)
		it.second->OnRefreshInstance();

	for(auto it : this->camMapping)
		it.second->OnRefreshInstance();
}

void DashboardGridInst::RemapInstance(const std::string& guidOld, const std::string& guidNew)
{
	this->Grid()->RemapInstance(guidOld, guidNew, this->bridge);
}

void DashboardGridInst::ToggleUIs(bool show)
{
	for(auto it : this->instMapping)
	{
		// Sanity check
		if(it.second->uiImpl == nullptr)
			continue;

		it.second->uiImpl->Toggle(show);
	}
	for(auto it : this->camMapping)
	{
		it.second->uiWindow->Show(show);

		if(show)
			it.second->uiWindow->Redraw();
	}
}