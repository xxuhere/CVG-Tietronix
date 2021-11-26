#pragma once
#include "DashboardGrid.h"
#include <map>

class wxWindow;
class CVGBridge;
class DashboardElementInst;

class DashboardGridInst
{
	friend DashboardGrid;

protected:
	wxWindow* gridWindow;

	DashboardGrid * grid;

	CVGBridge * bridge;

	std::map<DashboardElement*, DashboardElementInst*> instMapping;

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

	void MatchEleInstLayouts();

	bool MatchEleInstLayout(DashboardElement* ele);

	DashboardElementInst* AddDashboardElement(
		int cellX, 
		int cellY, 
		int cellWidth, 
		int cellHeight, 
		const std::string& eqGUID, 
		CVG::ParamSPtr param,
		const std::string& uiImpl = "");

	bool HasImpl(DashboardElement * ele);

	DashboardElementInst* Implement(DashboardElement* ele);

	bool UpdateParamValue(const std::string& eqGuid, const std::string& paramId);

	bool RemoveElement(DashboardElement * ele, bool deleteElement);

	bool MoveCell(DashboardElement* ele, const wxPoint& pos, const wxSize& size);

	/// <summary>
	/// For all widgets in the grid, recreate the connections to the UI (or just
	/// recreate the UIs from scratch - if needed) to reconnect dead connections
	/// that can be reestablished.
	/// </summary>
	void RefreshInstances();

	void RemapInstance(const std::string& guidOld, const std::string& guidNew);
};