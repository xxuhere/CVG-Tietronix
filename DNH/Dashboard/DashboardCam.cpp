#include "DashboardCam.h"
#include "DashboardGrid.h"

DashboardCam::DashboardCam(
	DashboardGrid* owner,
	const std::string& eqGuid,
	CamChannel camChan)
	: DashboardTile(owner, eqGuid)
{
	this->port		= camChan.port;
	this->endpoint	= camChan.endpoint;
	this->scheme	= camChan.proto;

	this->UpdateURIFromHostname(camChan.hostname);
}

bool DashboardCam::SetDimensions(
	const wxPoint& pt, 
	const wxSize& sz, 
	bool checkCollisions)
{
	// !TODO: Duplicate of DashboardElement

	if(checkCollisions == true)
	{ 
		if(!this->gridOwner->AreCellsFree(pt, sz, this))
			return false;
	}

	int cellDim			= this->gridOwner->GridCellSize();

	this->gridPos		= pt;
	this->gridSize		= sz;
	this->cachedPos		= pt * cellDim;
	this->cachedSize	= wxSize(sz.x * cellDim, sz.y * cellDim);

	const int resrvNameHgt = 20;
	const int bottomPad = 5;

	// The uiPixel dimensions are within the pixel boundaries
	this->uiPixelPos = this->cachedPos;
	this->uiPixelSize = this->cachedSize;

	// Apply name and border padding
	static const int HBORDER = 2;
	this->uiPixelPos.x += HBORDER;
	this->uiPixelSize.x -= HBORDER * 2;
	this->uiPixelPos.y += resrvNameHgt;
	this->uiPixelSize.y -= (resrvNameHgt + bottomPad);

	return true;
}

void DashboardCam::SetLabel(const std::string& label)
{
	DashboardTile::SetLabel(label);
}

void DashboardCam::UpdateURIFromHostname(const std::string& hostname)
{
	this->uri = this->URIFromHostname(hostname);
}

std::string DashboardCam::URIFromHostname(const std::string& hostname) const
{
	return 
		this->scheme + 
		"://" + 
		hostname + 
		":" + 
		this->PortStr() + 
		"/" + 
		this->endpoint;
}

DashboardTile::Type DashboardCam::GetType()
{
	return Type::Cam;
}

DashboardTile* DashboardCam::Clone()
{
	return new DashboardCam(*this);
}