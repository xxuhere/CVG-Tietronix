#pragma once
#include <wx/wx.h>

class IDashEle;
class CVGBridge;
class DashboardGridInst;
class DashboardGrid;

/// <summary>
/// A tile in a dashboard.
/// </summary>
class DashboardTile
{
	friend DashboardGrid;
public:
	/// <summary>
	/// The type of tile.
	/// </summary>
	enum class Type
	{
		/// <summary>
		/// The tile is a parameter.
		/// 
		/// i.e., DashboardElement
		/// </summary>
		Param,

		/// <summary>
		/// The tile is a camera.
		/// 
		/// i.e., DashboardCam
		/// </summary>
		Cam
	};

protected:
	/// <summary>
	/// The equipment ID of the parameter.
	/// </summary>
	std::string guid;

	/// <summary>
	/// The cached equipment of the parameter, when connecting an existing
	/// element to a new connection.
	/// </summary>
	std::string purpose;

	/// <summary>
	/// The position on the dashboard, in grid cell units.
	/// </summary>
	wxPoint gridPos;

	/// <summary>
	/// The size on the dashboard, in grid cell units.
	/// </summary>
	wxSize gridSize;

	/// <summary>
	/// The position of the element on the canvas, in pixels.
	/// </summary>
	wxPoint cachedPos;

	/// <summary>
	/// The size of the element on the canvas, in pixels.
	/// </summary>
	wxSize cachedSize;

	// !TODO: Check if uiPixelPos should be removed, might be 
	// redundant with cachedPos
	wxPoint uiPixelPos;

	wxSize uiPixelSize;

	/// <summary>
	/// Reference to grid.
	/// </summary>
	DashboardGrid* gridOwner = nullptr;

	/// <summary>
	/// The label currently or last used. By default this will be the 
	/// Param's label, but may be changed.
	/// </summary>
	std::string label;

public:
	DashboardTile(DashboardGrid * gridOwner, const std::string& eqGuid);

	/// <summary>
	/// Set the position and dimensions of the tile.
	/// </summary>
	/// <param name="pt">The new position of the tile.</param>
	/// <param name="sz">The new dimensions of the tile.</param>
	/// <param name="checkCollisions">
	/// If true, the operation will only succeed if it doesn't
	/// collide with another child in the grid.
	/// </param>
	/// <returns>
	/// True if success. False if the position or dimensions collided
	/// or were out of bounds.
	/// </returns>
	virtual bool SetDimensions(const wxPoint& pt, const wxSize& sz, bool checkCollisions = true) = 0;

	/// <summary>
	/// Set the label of the tile.
	/// </summary>
	/// <param name="label">The new tile label.</param>
	virtual void SetLabel(const std::string& label);

	inline const std::string& EqGUID() const
	{ return this->guid; }

	inline std::string Label() const
	{ return this->label; }

	inline wxPoint CellPos() const
	{ return this->gridPos; }

	inline wxSize CellSize() const
	{ return this->gridSize; }

	inline wxPoint PixelPos() const
	{ return this->cachedPos; }

	inline wxSize PixelSize() const
	{ return this->cachedSize; }

	inline wxPoint UIPos() const
	{ return this->uiPixelPos; }

	inline wxSize UISize() const
	{ return this->uiPixelSize; }

	inline const std::string& EqPurpose() const
	{return this->purpose; }

	inline wxPoint CachedSize() const;

	/// <summary>
	/// Get the tile type.
	/// </summary>
	virtual Type GetType() = 0;

	/// <summary>
	/// Query if a cell occupies a specific tile.
	/// </summary>
	/// <param name="cell">The tile coordinate to query.</param>
	/// <returns>True if the cell overlaps queried point.</returns>
	bool CellInTile(const wxPoint& cell);

	/// <summary>
	/// Create a deep copy of the tile.
	/// </summary>
	/// <returns></returns>
	virtual DashboardTile* Clone() = 0;

	virtual ~DashboardTile();
};