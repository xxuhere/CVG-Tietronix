#pragma once

#include <string>
#include "DashboardTile.h"
#include "Params/Param.h"
#include "CamChannel.h"

class DashboardGrid;

class DashboardCam : public DashboardTile
{
protected:
	/// <summary>
	/// A cached version of the entire uri
	/// </summary>
	std::string uri;

	/// <summary>
	/// The port portion of the URI
	/// </summary>
	int port;

	/// <summary>
	/// The scheme portion of the URI
	/// </summary>
	std::string scheme;

	/// <summary>
	/// The endpoint portion of the URI
	/// </summary>
	std::string endpoint;

public:
	DashboardCam(
		DashboardGrid* owner,  
		const std::string& eqGuid, 
		CamChannel camChan);

	std::string URI() const
	{ return this->uri; }

	int Port() const
	{ return this->port; }

	std::string PortStr() const
	{ return std::to_string(this->port); }

	std::string Scheme() const
	{ return this->scheme; }

	std::string Endpoint() const
	{ return this->endpoint; }

	bool SetDimensions(const wxPoint& pt, const wxSize& sz, bool checkCollisions = true) override;

	void SetLabel(const std::string& label) override;

	void UpdateURIFromHostname(const std::string& hostname);

	std::string URIFromHostname(const std::string& hostname) const;

	Type GetType() override;

	DashboardTile* Clone() override;
};