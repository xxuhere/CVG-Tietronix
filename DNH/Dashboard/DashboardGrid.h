#pragma once
#include <vector>
#include <map>
#include <set>
#include "DashboardElement.h"
#include "CVGBridge.h"

class DashboardGridInst;

/// <summary>
/// The document storing a dashboard.
/// </summary>
class DashboardGrid
{
	friend DashboardGridInst;

public:
	struct GUIDPurposePair
	{
		std::string guid;
		std::string purpose;
	};

	enum class RemapRet
	{
		Illegal,
		NoChange,
		Success
	};

private:
	class DashGroup
	{
	private:
		/// <summary>
		/// The cached GUID value.
		/// </summary>
		std::string eqGUID;

		std::string eqPurpose;

		/// <summary>
		/// The group of UI parameter implementations
		/// in the DashboardGrid for the Group's particular
		/// equipment.
		/// </summary>
		std::vector<DashboardElement*> group;

	public:
		DashGroup(const std::string& eqGUID, const std::string& eqPurpose);

		inline std::string EqGUID() const
		{ return this->eqGUID; }

		inline std::string EqPurpose() const
		{ return this->eqPurpose; }

		inline int Size() const
		{ return this->group.size();}

		inline DashboardElement * GetElement(int idx)
		{ return this->group[idx]; }

		DashboardElement* Find(const std::string& paramId);

		bool Forget(const std::string& paramId);

		bool Add(DashboardElement* ele);

		bool Destroy(const std::string& paramId);
		bool Destroy(DashboardElement* ele);

		bool Contains(const std::string& paramId);

		void Reset(const std::string& eqGUID, const std::string& eqPurpose);
	};

	// The max grid width
	int gridWidth = 100;

	// The max grid height
	int gridHeight = 100;

	std::vector<DashboardElement *> elements;

	std::map<std::string, DashGroup*> equipmentGrouping;

	int gridCellSize;

public:
	std::string name;

public:
	DashboardGrid(int gridCellSize, const std::string& name);
	DashboardGrid(DashboardGrid* deepCopyTarg);
	~DashboardGrid();

	inline int GridCellSize() const
	{ return this->gridCellSize; }

	inline int CellWidth() const
	{ return this->gridWidth; }

	inline int CellHeight() const
	{ return this->gridHeight; }

	// Begin and end iterator exposed to allow ranged-for loops 
	// on the DashoardGrid object.
	inline std::vector<DashboardElement *>::iterator begin()
	{
		return this->elements.begin();
	}
	//
	inline std::vector<DashboardElement *>::iterator end()
	{
		return this->elements.end();
	}

	inline std::vector<DashboardElement *>::const_iterator begin() const
	{
		return this->elements.begin();
	}
	//
	inline std::vector<DashboardElement *>::const_iterator end() const
	{
		return this->elements.end();
	}

	void RedoDashLayout();

	void Clear();

	bool Remove(const std::string& guid, const std::string& paramID);
	bool Remove(DashboardElement* de);

	bool Contains(const std::string& guid, const std::string& paramID) const;

	bool ContainsDashbordElement(DashboardElement* de) const;

	bool AreCellsFree(const wxPoint& gridPt, const wxSize& gridSz);
	bool AreCellsFree(const wxPoint& gridPt, const wxSize& gridSz, DashboardElement* ignore);
	bool AreCellsFree(const wxPoint& gridPt, const wxSize& gridSz, std::set<DashboardElement*> ignores);

	bool InDocumentBounds(const wxPoint& gridPt, const wxSize& gridSz) const;

	DashboardElement* AddDashboardElement(
		const std::string& eqGuid, 
		const std::string& eqPurpose,
		CVG::ParamSPtr ptr, 
		int gridX,
		int gridY,
		const std::string& uiImplName = "",
		int gridWidth = 4, 
		int gridHeight = 1,
		bool allowOverlap = false);

	wxPoint ConvertPixelsToGridCell(const wxPoint& pixel);

	DashboardElement* GetDashboardAtCell(const wxPoint& cell);

	DashboardElement* GetDashboardAtPixel(const wxPoint& pixel);

	bool MoveCell(DashboardElement* toMove, const wxPoint& newPos, const wxSize& newSize, bool collisionCheck = true);

	std::vector<GUIDPurposePair> GetEquipmentList() const;

	RemapRet RemapInstance(const std::string& guidOld, const std::string& guidNew, CVGBridge * bridge);

	void RefreshAllParamInstances(CVGBridge* bridge);
};