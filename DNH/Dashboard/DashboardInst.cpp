#include "DashboardInst.h"

DashboardInst::DashboardInst(
	DashboardGridInst*	instOwner,
	CVGBridge*			bridge,
	DashboardTile*		tile )
{
	this->instOwner = instOwner;
	this->bridge	= bridge;
	this->tile		= tile;
}

bool DashboardInst::LayoutUIImpl()
{
	// Do-nothing at the moment.
	// May be better suited as a pure virtual.
	return false;
}

void DashboardInst::OnRefreshInstance()
{
	// Do-nothing at the moment.
	// May be better suited as a pure virtual.
}

bool DashboardInst::DestroyUIImpl()
{
	this->instOwner = nullptr;
	this->bridge	= nullptr;
	this->tile		= nullptr;
	return true;
}

void DashboardInst::DrawImplPreview(wxPaintDC& dc, const wxPoint& offset)
{
	// Do-nothing at the moment.
	// May be better suited as a pure virtual.
}