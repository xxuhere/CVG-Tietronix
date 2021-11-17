#pragma once
#include <vector>
#include <map>
#include "DashboardElement.h"

// NOT USED YET - Will be the backend datastructure
// for a dashboard.
class DashboardGrid
{

	class DashGroup
	{
	private:
		/// <summary>
		/// The cached GUID value.
		/// </summary>
		std::string eqGUID;

		/// <summary>
		/// The group of UI parameter implementations
		/// in the DashboardGrid for the Group's particular
		/// equipment.
		/// </summary>
		std::vector<DashboardElement*> group;

	public:
		DashGroup(const std::string& eqGUID);

		inline const std::string & EqGUID() const
		{ return this->eqGUID; }

		inline int Size() const
		{ return this->group.size();}

		DashboardElement* Find(const std::string& paramId);
		bool Forget(const std::string& paramId);
		bool Add(DashboardElement* ele);
		bool Destroy(const std::string& paramId);
		bool Contains(const std::string& paramId);
	};

	std::vector<DashboardElement * > elements;
	std::map<std::string, DashGroup*> equipmentGrouping;

public:
	void RedoDashLayout();
	void Clear();
	bool Remove(const std::string& guid, const std::string& paramID);
	bool Contains(const std::string& guid, const std::string& paramID) const;
	bool ContainsDashbordElement(DashboardElement* de) const;
	bool AddDashboardElement(DashboardElement* de);
};