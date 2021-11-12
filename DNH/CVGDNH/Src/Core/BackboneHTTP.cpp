#include "BackboneHTTP.h"
#include "nlohmann/json.hpp"
#include "CoreDNH.h"

// Used to get system username
#include <boost/asio/ip/host_name.hpp>

using json = nlohmann::json;

namespace CVG
{
	BackboneHTTP::BackboneHTTP(int port, CoreDNH* coreSys)
	{
		this->server.config.port = port;
		this->coreSys = coreSys;

		// Setup endpoint system to be used by the callback function.
		this->server.resource["^/system$"]["GET"] = 
			[this](
				std::shared_ptr<HttpServer::Response> response,
				std::shared_ptr<HttpServer::Request> request)
			{
				this->HandleGETEndpoint_System(response, request);
			};

		// Setup endpoint equipment to be used by the callback function.
		this->server.resource["^/equipment$"]["GET"] =
			[this](
				std::shared_ptr<HttpServer::Response> response,
				std::shared_ptr<HttpServer::Request> request)
			{
				this->HandleGETEndpoint_Equipment(response, request);
			};
	}

	bool BackboneHTTP::StartServer()
	{
		// Any state where it's running or non-idle is not in a position
		// to accept a StartServer() request.
		if (this->status != BackboneStatus::DiedFromError &&
			this->status != BackboneStatus::Disconnected)
		{
			return false;
		}

		this->status = BackboneStatus::Starting;
		this->server_thread =
			std::thread(
				[this]()
				{
					// There's technically a miniscule moment from when we set the
					// state to running, and when it's actually running.
					this->status = BackboneStatus::Running;
					this->server.start();
					
					// The server.start() function will block the thread. It should 
					// only end when the the Backbone stops it.
					// If that's not the case, we need to set the state to reflect
					// this unexpected operation.
					//
					// NOTE: We may need to also formally log the issue.
					if (this->status == BackboneStatus::ShuttingDown)
						this->status = BackboneStatus::Disconnected;
					else
						this->status = BackboneStatus::UnknownError;
				});

		return true;
	}

	bool BackboneHTTP::ShutdownServer()
	{
		if (this->status != BackboneStatus::Starting &&
			this->status != BackboneStatus::Running)
		{
			return false;
		}

		this->status = BackboneStatus::ShuttingDown;
		this->server.stop();
		this->server_thread.join();

		// Note that returning a true doesn't mean the server 
		// is now shut down, just that the shutdown process was
		// successfully initiated.
		//
		// To know when the server is successfully shut down, 
		// we need to check that the server-thread set the status
		// to Disconnected.
		return true;
	}

	bool BackboneHTTP::IsRunning()
	{
		return this->status == BackboneStatus::Running;
	}

	void BackboneHTTP::HandleGETEndpoint_System(
		std::shared_ptr<HttpServer::Response> response,
		std::shared_ptr<HttpServer::Request> request)
	{
		json j = this->coreSys->GenerateJSON_System();
		std::string jsonRet = j.dump(4);
		response->write(jsonRet);
	}

	void BackboneHTTP::HandleGETEndpoint_Equipment(
		std::shared_ptr<HttpServer::Response> response,
		std::shared_ptr<HttpServer::Request> request)
	{
		json j = this->coreSys->GenerateJSON_Equipment();
		std::string jsonRet = j.dump(4);
		response->write(jsonRet);
	}
}