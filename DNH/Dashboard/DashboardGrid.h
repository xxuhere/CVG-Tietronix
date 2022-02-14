#pragma once
#include <vector>
#include <map>
#include <set>
#include "DashboardElement.h"
#include "DashboardCam.h"
#include "CVGBridge.h"

class DashboardGridInst;

/// <summary>
/// The document storing a dashboard.
/// </summary>
// TODO: Consider renaming (DashboardDoc)
class DashboardGrid
{
	friend DashboardGridInst;

public:

	/// <summary>
	/// A pairing of an Equipment guid and its purpose.
	/// </summary>
	struct GUIDPurposePair
	{
		/// <summary>
		/// The GUID to map to a specific Equipment instance during runtime.
		/// </summary>
		std::string guid;

		/// <summary>
		/// The purpose to map to an Equipment outside of runtime.
		/// </summary>
		std::string purpose;
	};

	/// <summary>
	/// The results for RemapInstance.
	/// </summary>
	enum class RemapRet
	{
		/// <summary>
		/// Error, the request was invalid.
		/// </summary>
		Illegal,

		/// <summary>
		/// The request resulted in no change.
		/// </summary>
		NoChange,

		/// <summary>
		/// The request was perfomed successfully.
		/// </summary>
		Success
	};

private:

	/// <summary>
	/// A grouping of tiles in the DashboardGrid that share
	/// the same Equipment.
	/// </summary>
	class DashGroup
	{
	private:
		/// <summary>
		/// The cached GUID value of the Equipment.
		/// </summary>
		std::string eqGUID;

		/// <summary>
		/// The cached purpose of the the Equipment.
		/// </summary>
		std::string eqPurpose;

		/// <summary>
		/// The tiles in the DashboardGrid that belong to the Equipment.
		/// </summary>
		std::vector<DashboardTile*> tiles;
	public:

		DashGroup(const std::string& eqGUID, const std::string& eqPurpose);

		inline std::string EqGUID() const
		{ return this->eqGUID; }

		inline std::string EqPurpose() const
		{ return this->eqPurpose; }

		inline int Size() const
		{ return this->tiles.size();}

		inline DashboardTile * GetTile(int idx)
		{ return this->tiles[idx]; }

		/// <summary>
		/// Find a tile that is mapped to a specific parameter ID.
		/// </summary>
		/// <param name="paramId">The ID of the parameter to find..</param>
		/// <returns>The found tile, or nullptr if none was found.</returns>
		DashboardTile* Find(const std::string& paramId);

		/// <summary>
		/// Unregister the tile that is mapped to a specific parameter ID.
		/// </summary>
		/// <param name="paramId">The ID of the parameter to unregister.</param>
		/// <returns>True if success.</returns>
		bool Forget(const std::string& paramId);

		/// <summary>
		/// Add a new dashboard tile.
		/// </summary>
		/// <param name="tile">The tile being added.</param>
		/// <returns>True if success.</returns>
		bool Add(DashboardTile* tile);

		/// <summary>
		/// Destroy a parameter tile contained in the DashGroup.
		/// 
		/// This is both a deletion of the tile, as well as a Forget().
		/// </summary>
		/// <param name="paramId">The ID of the parameter being destroyed.</param>
		/// <returns>True if successful.</returns>
		bool Destroy(const std::string& paramId);

		/// <summary>
		/// Destroy a tile contained in the DashGroup.
		/// 
		/// This is both a deletion of the tile, as well as a Forget().
		/// </summary>
		/// <param name="tile">The tile to delete.</param>
		/// <returns>True if successful.</returns>
		bool Destroy(DashboardTile* tile);

		/// <summary>
		/// Query if a parameter tile is contained in the DashGroup.
		/// </summary>
		/// <param name="paramId">The ID of the parameter to query.</param>
		/// <returns>True if the tile is contained.</returns>
		bool Contains(const std::string& paramId);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="eqGUID"></param>
		/// <param name="eqPurpose"></param>
		void Reset(const std::string& eqGUID, const std::string& eqPurpose);
	};

	// The max grid width
	int gridWidth = 100;

	// The max grid height
	int gridHeight = 100;

	/// <summary>
	/// A listing of all the tiles. This should be in sync
	/// with the data in equipmentGrouping.
	/// </summary>
	std::vector<DashboardTile *> tiles;

	/// <summary>
	/// A listing of all the tiles grouped by equipment. This
	/// should be in sync with the data in tile.
	/// </summary>
	std::map<std::string, DashGroup*> equipmentGrouping;

	/// <summary>
	/// The number of pixels that define the width and height
	/// of a grid cell.
	/// </summary>
	int gridCellSize;

public:
	/// <summary>
	/// The authored name of the DashboardGrid.
	/// </summary>
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
	inline std::vector<DashboardTile *>::iterator begin()
	{
		return this->tiles.begin();
	}
	//
	inline std::vector<DashboardTile *>::iterator end()
	{
		return this->tiles.end();
	}
	//
	inline std::vector<DashboardTile *>::const_iterator begin() const
	{
		return this->tiles.begin();
	}
	//
	inline std::vector<DashboardTile *>::const_iterator end() const
	{
		return this->tiles.end();
	}

	// !TODO: Unimplemented and unknown, remove.
	void RedoDashLayout();

	/// <summary>
	/// Clear all the contents.
	/// </summary>
	void Clear();

	/// <summary>
	/// Remove a param from the document.
	/// </summary>
	/// <param name="guid">The GUID to remove</param>
	/// <param name="paramID"></param>
	/// <returns></returns>
	bool Remove(const std::string& guid, const std::string& paramID);

	/// <summary>
	/// Remove a tile from the document.
	/// </summary>
	/// <param name="tile">The tile to remove.</param>
	/// <returns>True if success.</returns>
	bool Remove(DashboardTile* tile);

	/// <summary>
	/// Query if a Param is contained in the document.
	/// </summary>
	/// <param name="guid">The GUID of the Equipment the parameter belongs to.</param>
	/// <param name="paramID">The ID of the parameter to query for.</param>
	/// <returns>True if the Param was found in the document.</returns>
	bool Contains(const std::string& guid, const std::string& paramID) const;

	/// <summary>
	/// Query if a Param is contained in the document.
	/// </summary>
	/// <param name="de">The Param to query for.</param>
	/// <returns>True if the Param was found in the document.</returns>
	bool ContainsDashboardElement(DashboardElement* de) const;

	/// <summary>
	/// Check if a region of the document if empty.
	/// </summary>
	/// <param name="gridPt">The top left of the query region, in cell units.</param>
	/// <param name="gridSz">The dimensions of the query region, in cell units.</param>
	/// <returns>If true, the queried region does not overlap any tiles.</returns>
	bool AreCellsFree(const wxPoint& gridPt, const wxSize& gridSz);

	/// <summary>
	/// Check if a region of the document if empty.
	/// </summary>
	/// <param name="gridPt">The top left of the query region, in cell units.</param>
	/// <param name="gridSz">The dimensions of the query region, in cell units.</param>
	/// <param name="ignore">A tile that should be ignored from the query.</param>
	/// <returns>If true, the queried region does not overlap any tiles.</returns>
	bool AreCellsFree(const wxPoint& gridPt, const wxSize& gridSz, DashboardTile* ignore);

	/// <summary>
	/// Check if a region of the document if empty.
	/// </summary>
	/// <param name="gridPt">The top left of the query region, in cell units.</param>
	/// <param name="gridSz">The dimensions of the query region, in cell units.</param>
	/// <param name="ignores">A set of tiles that should be ignored from the query.</param>
	/// <returns>If true, the queried region does not overlap any tiles.</returns>
	bool AreCellsFree(const wxPoint& gridPt, const wxSize& gridSz, std::set<DashboardTile*> ignores);

	/// <summary>
	/// Check if a region is completly in the bound of the document.
	/// </summary>
	/// <param name="gridPt">The top left of the query region, in cell units.</param>
	/// <param name="gridSz">The dimensions of the query region, in cell units.</param>
	/// <returns>If true, the queried region fits inside the document.</returns>
	bool InDocumentBounds(const wxPoint& gridPt, const wxSize& gridSz) const;

	/// <summary>
	/// Create a new Param tile in the document.
	/// </summary>
	/// <param name="eqGuid">The GUID of the Param's Equipment.</param>
	/// <param name="eqPurpose">The purpose of the Param's Equipment.</param>
	/// <param name="ptr">The Param.</param>
	/// <param name="gridX">The grid cell position's X component.</param>
	/// <param name="gridY">The grid cell position's Y component.</param>
	/// <param name="uiImplName">
	/// The implementation of the Param's UI. Leave empty for the param's default implementation.
	/// </param>
	/// <param name="gridWidth">The width of the tile, in cell units.</param>
	/// <param name="gridHeight">The height of the tile, in cell units.</param>
	/// <param name="allowOverlap">
	/// If true, ignore the Add* request and return nullptr if the 
	/// specified creation region overlaps another tile in the document.
	/// </param>
	/// <returns>The created Param tile.</returns>
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

	/// <summary>
	/// Create a new camera tile in the document.
	/// </summary>
	/// <param name="eqGuid">The GUID of the Equipment the camera is registered to.</param>
	/// <param name="eqPurpose">The purpose of the Equipment the camera is registered to.</param>
	/// <param name="gridX">The grid cell position's X component.</param>
	/// <param name="gridY">The grid cell position's Y component.</param>
	/// <param name="camChan">The camera URI information.</param>
	/// <param name="gridWidth">The width of the tile, in cell units.</param>
	/// <param name="gridHeight">The height of the tile, in cell units.</param>
	/// <param name="allowOverlap">
	/// If true, ignore the Add* request and return nullptr if the
	/// specified creation region overlaps another tile in the document.
	/// </param>
	/// <returns>The created camera tile.</returns>
	DashboardCam* AddDashboardCam(
		const std::string& eqGuid,
		const std::string& eqPurpose,
		int gridX,
		int gridY,
		CamChannel camChan,
		int gridWidth = 4, 
		int gridHeight = 1,
		bool allowOverlap = false);

	/// <summary>
	/// Convert a pixel position to a grid cell position.
	/// </summary>
	/// <param name="pixel">The pixel position.</param>
	/// <returns>The grid cell position.</returns>
	wxPoint ConvertPixelsToGridCell(const wxPoint& pixel);

	/// <summary>
	/// Check the tile in a grid cell.
	/// </summary>
	/// <param name="cell">The coordinate of the cell to query for.</param>
	/// <returns>
	/// The tile found at the coordinate, or nullptr if nothing was found.
	/// </returns>
	DashboardTile* GetTileAtCell(const wxPoint& cell);

	/// <summary>
	/// Check the tile in a pixel position.
	/// </summary>
	/// <param name="pixel">The pixel position to query for.</param>
	/// <returns>The tile found at the pixel, or nullptr if nothing was found.</returns>
	DashboardTile* GetTileAtPixel(const wxPoint& pixel);

	/// <summary>
	/// Move and resize a cell.
	/// </summary>
	/// <param name="toMove">The tile to move.</param>
	/// <param name="newPos">The new position, in cell units.</param>
	/// <param name="newSize">The new position, in cell units.</param>
	/// <param name="collisionCheck">
	/// If true ignore the operation and return false if the new
	/// tile position and dimensions will overlap another tile.
	/// </param>
	/// <returns>
	/// True if success. The move will fail if there is an overlap with
	/// the collisionCheck set, or if the move results in an invalid size,
	/// or if the tile will end up out of bounds.
	/// </returns>
	bool MoveCell(DashboardTile* toMove, const wxPoint& newPos, const wxSize& newSize, bool collisionCheck = true);

	/// <summary>
	/// Get a list of all Equipments referenced in the document.
	/// </summary>
	/// <returns></returns>
	std::vector<GUIDPurposePair> GetEquipmentList() const;

	/// <summary>
	/// For all tiles referencing a GUID, switch that reference with
	/// another GUID.
	/// 
	/// This is often used to remap a broken Equipment reference with
	/// another active Equipment that shares the same purpose value.
	/// </summary>
	/// <param name="guidOld">The old GUID to be replaced.</param>
	/// <param name="guidNew">The new GUID to replace the old one.</param>
	/// <param name="bridge">The reference to the application instance.</param>
	/// <returns>Success status.</returns>
	RemapRet RemapInstance(const std::string& guidOld, const std::string& guidNew, CVGBridge * bridge);

	/// <summary>
	/// Refresh all parameter instances.
	/// </summary>
	/// <param name="bridge">Reference to the application interface.</param>
	void RefreshAllParamInstances(CVGBridge* bridge);

	/// <summary>
	/// Set the document's size, defined in tile units.
	/// </summary>
	/// <param name="sz">The tile dimensions of the dashboard.</param>
	/// <returns>True if success.</returns>
	bool SetCellSize(const wxSize& sz);
};