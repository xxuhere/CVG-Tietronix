#pragma once
#include "DashboardGrid.h"
#include <map>

class wxWindow;
class CVGBridge;
class DashboardElementInst;
class DashboardCamInst;
class DashboardInst;

class DashboardGridInst
{
	friend DashboardGrid;

protected:
	wxWindow* gridWindow;

	DashboardGrid * grid;

	CVGBridge * bridge;

	std::map<DashboardElement*, DashboardElementInst*> instMapping;
	std::map<DashboardCam*, DashboardCamInst*> camMapping;

public:
	class iterator
	{
	private:
		std::map<DashboardElement*, DashboardElementInst*>* pInstMap;
		std::map<DashboardElement*, DashboardElementInst*>::iterator itInstMap;

		std::map<DashboardCam*, DashboardCamInst*>* pCamMap;
		std::map<DashboardCam*, DashboardCamInst*>::iterator itCamMap;

	public:
		iterator(
			std::map<DashboardElement*, DashboardElementInst*>* pInstMap,
			std::map<DashboardElement*, DashboardElementInst*>::iterator itInstMap,
			std::map<DashboardCam*, DashboardCamInst*>* pCamMap,
			std::map<DashboardCam*, DashboardCamInst*>::iterator itCamMap);

		iterator(const iterator& it);
		~iterator();
		iterator& operator=(const iterator& it);
		iterator* operator++();
		std::pair<DashboardTile*, DashboardInst*> operator*() const;
		bool operator == (const iterator& it) const;
		bool operator != (const iterator& it) const;
	};

public:
	DashboardGridInst(
		wxWindow* gridCanvasWin,
		CVGBridge* bridge, 
		DashboardGrid* attachedGrid);

	~DashboardGridInst();

	inline wxWindow* GridWindow()
	{ return this->gridWindow; }

	DashboardGrid* Grid()
	{ return this->grid; }

	inline int CellWidth() const
	{ return this->grid->CellWidth(); }

	inline int CellHeight() const
	{ return this->grid->CellHeight(); }

	inline int GridCellSize() const
	{ return this->grid->GridCellSize(); }

	// Allow ranged-based for loops
	iterator begin();

	// Allow ranged-based for loops
	iterator end();

	void MatchEleInstLayouts();

	bool MatchEleInstLayout(DashboardElement* ele);
	bool MatchEleInstLayout(DashboardCam* cam);
	bool MatchEleInstLayout(DashboardInst* tileInst);

	DashboardElementInst* AddDashboardElement(
		int cellX, 
		int cellY, 
		int cellWidth, 
		int cellHeight, 
		const std::string& eqGUID, 
		CVG::ParamSPtr param,
		const std::string& uiImpl = "");

	DashboardCam* AddDashboardCam(
		int cellX, 
		int cellY, 
		int cellWidth, 
		int cellHeight, 
		const std::string& eqGUID, 
		CamChannel camChan);

	bool HasImpl(DashboardElement * ele);

	DashboardElementInst* Implement(DashboardElement* ele);
	DashboardCamInst* Implement(DashboardCam* cam);

	bool UpdateParamValue(const std::string& eqGuid, const std::string& paramId);

	bool RemoveTile(DashboardTile * tile, bool deleteTile);

	bool MoveCell(DashboardTile* tile, const wxPoint& pos, const wxSize& size);

	/// <summary>
	/// For all widgets in the grid, recreate the connections to the UI (or just
	/// recreate the UIs from scratch - if needed) to reconnect dead connections
	/// that can be reestablished.
	/// </summary>
	void RefreshInstances();

	void RemapInstance(const std::string& guidOld, const std::string& guidNew);

	void ToggleUIs(bool show);
};