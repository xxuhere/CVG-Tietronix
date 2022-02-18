#pragma once
#include <wx/wx.h>
#include "DashboardGridInst.h"

class CVGBridge;
class DashboardGridInst;
class Tile;

/// <summary>
/// Base class for a tile instance.
/// </summary>
class DashboardInst
{
private:
	/// <summary>
	/// Reference to the dashboard that the DashboardInst
	/// belongs to.
	/// </summary>
	DashboardGridInst* instOwner;

	/// <summary>
	/// Reference to the application's interface.
	/// </summary>
	CVGBridge* bridge;

	/// <summary>
	/// The tile whos UI is being instanciated.
	/// </summary>
	Tile* tile;

protected:
	inline wxWindow* GridCanvas()
	{ return this->GridInst()->GridWindow();}

public:
	DashboardInst(
		DashboardGridInst* instOwner,
		CVGBridge* bridge,
		Tile* tile);

	inline CVGBridge* Bridge()
	{ return this->bridge; }

	inline Tile* GetTile()
	{ return this->tile; }

	inline DashboardGridInst* GridInst()
	{ return this->instOwner; }

	/// <summary>
	/// Layout children UI (wxWindows) elements on the grid.
	/// 
	/// This should be called after any change to the tile instance's
	/// position or dimensions.
	/// </summary>
	/// <returns>
	/// True if success. The criteria for success is up to the
	/// implementation.
	/// </returns>
	virtual bool LayoutUIImpl();

	/// <summary>
	/// Refresh the implementation of the tile.
	/// 
	/// This should be called whenever a tile intance needs to
	/// reinitialize back-end systems.
	/// </summary>
	virtual void OnRefreshInstance();

	/// <summary>
	/// Destroy all UI content in the element.
	/// </summary>
	/// <returns>True if successful. Else, there was no UI content to destroy.</returns>
	virtual bool DestroyUIImpl();

	/// <summary>
	/// Redirect to DrawPreview without needing to directly
	/// access uiImpl.
	/// </summary>
	virtual void DrawImplPreview(wxPaintDC& dc, const wxPoint& offset);

	/// <summary>
	/// Signal sent when the application is connected from the DNH.
	/// </summary>
	virtual void OnConnect() = 0;

	/// <summary>
	/// Signal sent when the application is disconnected from the DNH.
	/// </summary>
	virtual void OnDisconnect() = 0;
};