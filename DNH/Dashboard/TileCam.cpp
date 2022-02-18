#include "TileCam.h"
#include "DashboardGrid.h"

TileCam::TileCam(
	DashboardGrid* owner,
	const std::string& eqGuid,
	CamChannel camChan)
	: Tile(owner, eqGuid)
{
	this->port		= camChan.port;
	this->endpoint	= camChan.endpoint;
	this->scheme	= camChan.proto;

	this->label = camChan.label;
	this->defaultLabel = camChan.label;

	this->UpdateURIFromHostname(camChan.hostname);
}

bool TileCam::SetDimensions(
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

void TileCam::SetLabel(const std::string& label)
{
	Tile::SetLabel(label);
}

void TileCam::UpdateURIFromHostname(const std::string& hostname)
{
	this->uri = this->URIFromHostname(hostname);
}

std::string TileCam::URIFromHostname(const std::string& hostname) const
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

Tile::Type TileCam::GetType()
{
	return Type::Cam;
}

Tile* TileCam::Clone()
{
	return new TileCam(*this);
}

std::string TileCam::DefaultLabel()
{
	return this->defaultLabel;
}