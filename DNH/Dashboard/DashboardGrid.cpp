#include "DashboardGrid.h"

DashboardGrid::DashGroup::DashGroup(const std::string& eqGUID)
{
	this->eqGUID = eqGUID;
}

DashboardElement* DashboardGrid::DashGroup::Find(const std::string& paramId)
{
	for (DashboardElement* ele : this->group)
	{
		if (ele->ParamID() == paramId)
			return ele;
	}
	return nullptr;
}

bool DashboardGrid::DashGroup::Forget(const std::string& paramId)
{
	for (
		auto it = this->group.begin();
		it != this->group.end();
		++it)
	{
		if ((*it)->ParamID() == paramId)
		{
			this->group.erase(it);
			return true;
		}
	}
	return false;
}

bool DashboardGrid::DashGroup::Add(DashboardElement* ele)
{
	if (this->Contains(ele->ParamID()))
		return false;

	this->group.push_back(ele);
	return true;
}

bool DashboardGrid::DashGroup::Destroy(const std::string& paramId)
{
	for (
		auto it = this->group.begin();
		it != this->group.end();
		++it)
	{
		if ((*it)->ParamID() == paramId)
		{
			(*it)->DestroyUIImpl();
			this->group.erase(it);
			return true;
		}
	}
	return false;
}

bool DashboardGrid::DashGroup::Contains(const std::string& paramId)
{
	return this->Find(paramId) != nullptr;
}

void DashboardGrid::RedoDashLayout()
{
}

void DashboardGrid::Clear()
{
	// Clear the vector of all items.
	for (DashboardElement* de : elements)
		de->DestroyUIImpl();

	this->elements.clear();

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
		auto it = this->elements.begin(); 
		it != this->elements.end(); 
		++it)
	{
		
		if ((*it)->EqGUID() == guid && (*it)->ParamID() == paramID)
		{
			// Just erase, it's already been destroyed above.
			this->elements.erase(it);
			return true;
		}
	}
	return false;
}

bool DashboardGrid::AddDashboardElement(DashboardElement* de)
{
	auto itFindGroup = 
		this->equipmentGrouping.find(de->EqGUID());

	DashGroup* eqGroup = nullptr;
	if (itFindGroup != this->equipmentGrouping.end())
	{
		eqGroup = itFindGroup->second;
	}
	else
	{
		// If we didn't find the group it will be placed in,
		// we create it.
		eqGroup = new DashGroup(de->EqGUID());
		this->equipmentGrouping[de->EqGUID()] = eqGroup;
	}

	eqGroup->Add(de);
	this->elements.push_back(de);
	return true;
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

bool DashboardGrid::ContainsDashbordElement(DashboardElement* de) const
{
	return this->Contains(de->EqGUID(), de->ParamID());
}