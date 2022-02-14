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
	Type GetType() override;
	DashboardTile* Clone() override;

	/// <summary>
	/// Update the URI from a specified hostname.
	/// </summary>
	/// <param name="hostname">The hostname.</param>
	void UpdateURIFromHostname(const std::string& hostname);

	/// <summary>
	/// Generate a URI using a specific hostname.
	/// 
	/// More specifically, this will create a URI using the
	/// stored port, scheme and endpoint, but uses a 
	/// parameter for the hostname.
	/// </summary>
	/// <param name="hostname">The hostname.</param>
	/// <returns>The URI if the specified hostname is used.</returns>
	std::string URIFromHostname(const std::string& hostname) const;

};