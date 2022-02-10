#pragma once
#include <wx/wx.h>
#include "DashboardGridInst.h"

class CVGBridge;
class DashboardGridInst;
class DashboardTile;

class DashboardInst
{
private:
	DashboardGridInst* instOwner;
	CVGBridge* bridge;
	DashboardTile* tile;

protected:
	inline wxWindow* GridCanvas()
	{ return this->GridInst()->GridWindow();}

public:
	DashboardInst(
		DashboardGridInst* instOwner,
		CVGBridge* bridge,
		DashboardTile* tile);

	inline CVGBridge* Bridge()
	{ return this->bridge; }

	inline DashboardTile* Tile()
	{ return this->tile; }

	inline DashboardGridInst* GridInst()
	{ return this->instOwner; }

	virtual bool LayoutUIImpl();

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
};