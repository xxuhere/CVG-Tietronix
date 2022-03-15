#pragma once
#include "CompatabilityHacks.h" // Suppress 4996

#include <string>
#include "BackboneStatus.h"
#include <Simple-Web-Server/server_http.hpp>
#include <memory>

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;

namespace CVG
{
	class CoreDNH;

	/// <summary>
	/// Handles the HTTP (REST) communication tasks of thh CoreDNH.
	/// </summary>
	class BackboneHTTP
	{
	private:

		// Reference to the DNH that owns this HTTP server component.
		CoreDNH* coreSys;

		// The server status, used for tracking the current state of the
		// server, as well as for checking the main thread and HTTP server
		// thread are interacting/handshaking as expected.
		BackboneStatus status = BackboneStatus::Disconnected;

		// If the HTTP encounters an error that involves shutting
		// it down, the reason will be stored here.
		//
		// NOTE: Not currently used, may not be relevant.
		std::string lastDiedError;

		// The HTTP server.
		HttpServer server;

		// The thread running the HTTP server.
		std::thread server_thread;

	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="port">HTTP server port.</param>
		/// <param name="coreSys">Reference to the DNH that owns this HTTP server.</param>
		BackboneHTTP(int port, CoreDNH* coreSys);

		inline BackboneStatus Status() { return this->status; }

		/// <summary>
		/// Start the server.
		/// </summary>
		/// <returns>
		/// True if successful. Else, false.
		/// 
		/// Starting the server will fail if IT settings are not valid, or
		/// if the server is not in a valid state to start (e.g., if it's
		/// already running).
		/// </returns>
		bool StartServer();

		/// <summary>
		/// Shutdown the server.
		/// </summary>
		/// <returns>
		/// True if successful. Else, false.
		/// 
		/// Shutdown will fail if the server is not in a valid state to stop,
		/// such as if it's not running.
		/// 
		/// A return value of true means the shutdown process has been
		/// successfully initated, but it may not be completely stopped
		/// when the call to ShutdownServer() returns.
		/// </returns>
		bool ShutdownServer();

		/// <summary>
		/// Query if the server is running.
		/// 
		/// This should not be confused with checking if the server is 
		/// disconnected. As it may be in a non-disconnected state such
		/// as connecting or in the middle of a shutdown process.
		/// </summary>
		/// <returns>True if the server is running, else false.</returns>
		bool IsRunning();

	public: // Endpoints
		
		/// <summary>
		/// HTTP request handler for the /system endpoint.
		/// </summary>
		void HandleGETEndpoint_System(
			std::shared_ptr<HttpServer::Response> response,
			std::shared_ptr<HttpServer::Request> request);

		/// <summary>
		/// HTTP request handler for the /equipment endpoint.
		/// </summary>
		void HandleGETEndpoint_Equipment(
			std::shared_ptr<HttpServer::Response> response,
			std::shared_ptr<HttpServer::Request> request);
	};
}