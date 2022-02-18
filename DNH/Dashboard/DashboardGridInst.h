#pragma once
#include "DashboardGrid.h"
#include <map>

class wxWindow;
class CVGBridge;
class DashboardElementInst;
class DashboardCamInst;
class DashboardInst;

/// <summary>
/// Base class for a dashboard grid instance.
/// 
/// This will implement a UI instance of a DashboardGrid.
/// </summary>
class DashboardGridInst
{
	friend DashboardGrid;

protected:
	/// <summary>
	/// The canvas window that the UI children are placed.
	/// </summary>
	wxWindow* gridWindow;

	/// <summary>
	/// The DashboardGrid whose UI is being instanced.
	/// </summary>
	DashboardGrid * grid;

	/// <summary>
	/// A reference to the application interface.
	/// </summary>
	CVGBridge * bridge;

	/// <summary>
	/// Parameter tiles.
	/// </summary>
	std::map<TileParam*, DashboardElementInst*> instMapping;

	/// <summary>
	/// Camera tiles.
	/// </summary>
	std::map<TileCam*, DashboardCamInst*> camMapping;

public:
	/// <summary>
	/// Iterator to iterate through all all DashboardInst contents 
	/// of the DashboardGridInst.
	/// </summary>
	class iterator
	{
	private:
		// Members to iterate the Param tile instances.
		std::map<TileParam*, DashboardElementInst*>* pInstMap;
		std::map<TileParam*, DashboardElementInst*>::iterator itInstMap;

		// Members to iterate the camera tile intances.
		std::map<TileCam*, DashboardCamInst*>* pCamMap;
		std::map<TileCam*, DashboardCamInst*>::iterator itCamMap;

	public:
		iterator(
			std::map<TileParam*, DashboardElementInst*>* pInstMap,
			std::map<TileParam*, DashboardElementInst*>::iterator itInstMap,
			std::map<TileCam*, DashboardCamInst*>* pCamMap,
			std::map<TileCam*, DashboardCamInst*>::iterator itCamMap);

		// C++ iterator functions.
		iterator(const iterator& it);
		~iterator();
		iterator& operator=(const iterator& it);
		iterator* operator++();
		std::pair<Tile*, DashboardInst*> operator*() const;
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

	/// <summary>
	/// Call LayoutUIImpl() for each tile in the
	/// DashboardGridInst.
	/// </summary>
	void MatchEleInstLayouts();

	/// <summary>
	/// Call LayoutUIImpl() for a specific tile instance in the 
	/// DashboardGridInst.
	/// </summary>
	/// <param name="tileInst">The tile to update the layout for.</param>
	/// <returns>True if success.</returns>
	bool MatchEleInstLayout(DashboardInst* tileInst);
	bool MatchEleInstLayout(TileParam* ele);
	bool MatchEleInstLayout(TileCam* cam);

	DashboardElementInst* AddDashboardElement(
		int cellX, 
		int cellY, 
		int cellWidth, 
		int cellHeight, 
		const std::string& eqGUID, 
		CVG::ParamSPtr param,
		const std::string& uiImpl = "");

	TileCam* AddDashboardCam(
		int cellX, 
		int cellY, 
		int cellWidth, 
		int cellHeight, 
		const std::string& eqGUID, 
		CamChannel camChan);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="ele">The Param to check for.</param>
	/// <returns>
	/// True if the grid contains a UI instance for the
	/// specified tile.
	/// </returns>
	bool HasImpl(TileParam * ele);

	/// <summary>
	/// Create an UI instance of a Param Tile.
	/// </summary>
	/// <param name="ele">The Param Tile to instantiate.</param>
	/// <returns>The created mapped instance.</returns>
	DashboardElementInst* Implement(TileParam* ele);
	
	/// <summary>
	/// Create a DashboardCamInst to instantiate a DashboardCam.
	/// </summary>
	/// <param name="cam">The DashboardCam to instantiate.</param>
	/// <returns>The created mapped instance.</returns>
	DashboardCamInst* Implement(TileCam* cam);

	/// <summary>
	/// Update the content in a UI for a specified Param.
	/// </summary>
	/// <param name="eqGuid">The GUID of the Equipment the Param belongs to.</param>
	/// <param name="paramId">The ID of the Param.</param>
	/// <returns>True if success.</returns>
	bool UpdateParamValue(const std::string& eqGuid, const std::string& paramId);

	/// See DashboardGrid.Remove() for details.
	bool RemoveTile(Tile * tile, bool deleteTile);

	/// See DashboardGrid.MoveCell() for details.
	bool MoveCell(Tile* tile, const wxPoint& pos, const wxSize& size);

	/// <summary>
	/// For all widgets in the grid, recreate the connections to the UI (or just
	/// recreate the UIs from scratch - if needed) to reconnect dead connections
	/// that can be reestablished.
	/// </summary>
	void RefreshInstances();

	/// <summary>
	/// See DashboardGrid.RemapInstance() for details.
	/// </summary>
	void RemapInstance(const std::string& guidOld, const std::string& guidNew);

	/// <summary>
	/// Toggle the visibility of all child tile UIs.
	/// </summary>
	/// <param name="show">
	/// Set to true to turn on all tile UIs. 
	/// Set to false to turn off all tile UIs.
	/// </param>
	void ToggleUIs(bool show);
};