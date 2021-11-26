#include "DashboardGridInst.h"
#include "DashboardElementInst.h"

DashboardGridInst::DashboardGridInst(
	wxWindow* gridCanvasWin,
	CVGBridge* bridge, 
	DashboardGrid* attachedGrid)
{
	this->gridWindow = gridCanvasWin;
	this->bridge = bridge;
	this->grid = attachedGrid;

	for(DashboardElement* ele : *attachedGrid)
	{
		DashboardElementInst* eleWrapper = 
			new DashboardElementInst(this, this->bridge, ele);

		this->instMapping[ele] = eleWrapper;

		if(!eleWrapper->SwitchUIImplementation(ele->GetUIImplName()))
		{ 
			eleWrapper->SwitchUIDefault();
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

	DashboardElement * dashEle = 
		this->grid->AddDashboardElement(
			eqGUID,
			eqPtr->Purpose(),
			param,
			cellX, 
			cellY,
			uiImpl,
			cellWidth, 
			cellHeight);

	if(dashEle == nullptr)
		return nullptr;

	return this->Implement(dashEle);
}

bool DashboardGridInst::HasImpl(DashboardElement * ele)
{
	auto it = this->instMapping.find(ele);
	return it != this->instMapping.end();
}

DashboardElementInst* DashboardGridInst::Implement(DashboardElement* ele)
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

bool DashboardGridInst::UpdateParamValue(const std::string& eqGuid, const std::string& paramId)
{
	auto itFind = this->grid->equipmentGrouping.find(eqGuid);
	if(itFind == this->grid->equipmentGrouping.end())
		return false;

	DashboardGrid::DashGroup* grp = itFind->second;
	const int ct = grp->Size();
	for(int i = 0; i < ct; ++i)
	{
		DashboardElement* ele = grp->GetElement(i);
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

bool DashboardGridInst::RemoveElement(DashboardElement * ele, bool deleteElement)
{
	auto it = this->instMapping.find(ele);
	if(it != this->instMapping.end())
	{
		DashboardElementInst* inst = it->second;
		inst->DestroyUIImpl();
		this->instMapping.erase(it);

		if(deleteElement == false)
			return true;
	}

	return this->grid->Remove(ele);
}

bool DashboardGridInst::MoveCell(DashboardElement* ele, const wxPoint& pos, const wxSize& size)
{
	if( !this->grid->MoveCell(ele, pos, size))
		return false;

	return this->MatchEleInstLayout(ele);
}

void DashboardGridInst::MatchEleInstLayouts()
{
	for(auto it : this->instMapping)
	{
		DashboardElementInst * eleInst = it.second;
		eleInst->LayoutUIImpl();
	}
}

bool DashboardGridInst::MatchEleInstLayout(DashboardElement* ele)
{
	auto it = this->instMapping.find(ele);
	if(it == this->instMapping.end())
		return false;

	DashboardElementInst* eleInst = it->second;
	return eleInst->LayoutUIImpl();
}

void DashboardGridInst::RefreshInstances()
{
	for(auto it : this->instMapping)
		it.second->OnRefreshInstance();
}

void DashboardGridInst::RemapInstance(const std::string& guidOld, const std::string& guidNew)
{
	this->Grid()->RemapInstance(guidOld, guidNew, this->bridge);
}