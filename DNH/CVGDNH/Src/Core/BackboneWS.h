#pragma once

#include <set>
#include <map>
#include <memory>
#include <boost/asio.hpp>
#include "SocketDefines.h"

#include "SEquipment.h"
#include "CVGData/Src/Types/ErrorTy.h"

using json = nlohmann::json;

namespace CVG
{
	class CoreDNH;

	/// <summary>
	/// The Websocket communications component of the DNH.
	/// 
	/// It's moved into its own subsystem so the DNH class
	/// and source file can be less concerned about the 
	/// nuances of the WebSocket server details.
	/// </summary>
	class BackboneWS
	{
	private:
		// Reference to the DNH that owns this WebSocket server component.
		CoreDNH* coreSys;

		// The WebSocket server.
		WsServer server;

		// The thread running the WebSocket server.
		std::thread server_thread;

		// The set of WebSocket client connections that are connected
		// to the server, but haven't performed a successful registration
		// yet.
		//
		// All successfully registered connections are converted to 
		// Equipments and registered with coreSys's Equipment list.
		std::set< WSConSPtr> unregistered;

		std::mutex unregisteredMutex;

		boost::asio::ip::address usableSelfIP;

	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="port">WebSocket server port.</param>
		/// <param name="coreSys">Reference to the DNH that owns this WebSocket server.</param>
		BackboneWS(int port, CoreDNH* coreSys);

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

		inline std::string SelfHostnameStr() const
		{ return this->usableSelfIP.to_string(); }

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

		/// <summary>
		/// Callback handler for reacting to WebSocket messages on the
		/// 'realtime' endpoint.
		/// </summary>
		/// <param name="connection">
		/// The connection to the WS client that sent the message.
		/// </param>
		/// <param name="message">The message received.</param>
		void HandleWS_Realtime_Message(
			std::shared_ptr<WsServer::Connection> connection,
			std::shared_ptr<WsServer::InMessage> message);

		/// <summary>
		/// Callback handler for reacting to a new WebSocket connection 
		/// being opened on the 'realtime' endpoint.
		/// </summary>
		/// <param name="connection">The connection to the new client.</param>
		void HandleWS_Realtime_Open(
			std::shared_ptr<WsServer::Connection> connection);

		/// <summary>
		/// Callback handler for reacting to a WebSocket disconnection.
		/// </summary>
		/// <param name="connection">The connection that was disconnected.</param>
		/// <param name="status">
		/// The WebSocket disconnection code for why the disconnection happened.
		/// </param>
		/// <param name="reason">
		/// The readable reason for why the disconnection happened.
		/// </param>
		void HandleWS_Realtime_Close(
			std::shared_ptr<WsServer::Connection> connection,
			int status,
			const std::string& reason);

		/// <summary>
		/// Callback handler for reacting to a WebSocket error.
		/// 
		/// TODO: It needs to be checked if all errors result in a disconnection,
		/// or if that is undefined.
		/// </summary>
		/// <param name="connection">The WebSocket connection where the error occured.</param>
		/// <param name="ec">The error code identifying the type of error.</param>
		void HandleWS_Realtime_Error(
			std::shared_ptr<WsServer::Connection> connection, 
			const SimpleWeb::error_code& ec);

		/// <summary>
		/// Remove a WebSocket connection. The function ensures the 
		/// connection is closed and removed entirely from the DNH
		/// network.
		/// </summary>
		/// <param name="con">The connection to remove.</param>
		/// <param name="reason">
		/// The reason to provide to the client why the disconnect
		/// occured - only relevant if the client is still connected.
		/// </param>
		void RemoveConnection(WSConSPtr con, const std::string& reason);

		// Realtime API handlers Each apity gets its own handler function.
		// See the dispatch mechanism in HandleWS_Realtime_Message()
		// for more information on how the functions are called.
		void HandleRTAPI_Register(WSConSPtr con, const json& js, const std::string & postage);
		void HandleRTAPI_System(WSConSPtr con, const json& js, const std::string& postage);
		void HandleRTAPI_Equipment(WSConSPtr con, const json& js, const std::string& postage);
		void HandleRTAPI_Status(WSConSPtr con, const json& js, const std::string& postage);
		void HandleRTAPI_ValGet(WSConSPtr con, const json& js, const std::string& postage);
		void HandleRTAPI_ValSet(WSConSPtr con, const json& js, const std::string& postage);
		void HandleRTAPI_Arm(WSConSPtr con, const json& js, const std::string& postage);
		void HandleRTAPI_Disarm(WSConSPtr con, const json& js, const std::string& postage);
		void HandleRTAPI_Publish(WSConSPtr con, const json& js, const std::string& postage);
		void HandleRTAPI_Sub(WSConSPtr con, const json& js, const std::string& postage);
		void HandleRTAPI_Purpose(WSConSPtr con, const json& js, const std::string& postage);
		void HandleRTAPI_Datacache(WSConSPtr con, const json& js, const std::string& postage);

		/// <summary>
		/// Broadcast a ping to all connections.
		/// 
		/// This also includes unregistered connections.
		/// </summary>
		void Ping();


	public: // Utilities

		/// <summary>
		/// Generate a WebSocket error response, formatted as a string.
		/// </summary>
		/// <param name="error">The error elevation type.</param>
		/// <param name="reason">A readable reason the error occured.</param>
		/// <param name="request">
		/// Optional. If the error occured in reponse to a request, the value
		/// should be the name of the request.
		/// </param>
		/// <param name="postage">
		/// Optional. Postage value to allow clients to match the errors 
		/// with a specific request.
		/// </param>
		/// <returns>The JSON error represented as a string.</returns>
		static std::string GenerateJSONError(
			ErrorTy error, 
			const std::string& reason, 
			const std::string& request,
			const std::string& postage = "");

		/// <summary>
		/// Send a WebSocket error to a connection.
		/// </summary>
		/// <param name="con">The connection to send the error to.</param>
		/// <param name="error">The error elevation type.</param>
		/// <param name="reason">A readable reason the error occured.</param>
		/// <param name="request">
		/// Optional. If the error occured in reponse to a request, the value
		/// should be the name of the request.
		/// </param>
		/// <param name="postage">
		/// Optional. Postage value to allow clients to match the errors 
		/// with a specific request.
		/// </param>
		static void SendError(
			WSConSPtr con,
			ErrorTy error,
			const std::string& reason,
			const std::string& request,
			const std::string& postage);

		/// <summary>
		/// Send a string to a WebSocket connection.
		/// </summary>
		/// <param name="con">The connection to send the data to.</param>
		/// <param name="payload">The data payload.</param>
		static void SendString(WSConSPtr con, const std::string& payload);

		/// <summary>
		/// Utility function to send JSON as a string across a WebSocket connection.
		/// </summary>
		/// <param name="con">The WebSocket connection to send the message to.</param>
		/// <param name="js">The JSON payload to send.</param>
		/// <param name="indent">The amount to indent the JSON text.</param>
		static void SendJSON(WSConSPtr con, const json & js, int indent = -1);

		/// <summary>
		/// Parse a query to register Equipment.
		/// </summary>
		/// <param name="js">The JSON representing the Equipment registration.</param>
		/// <param name="topicsOut">
		/// The topics that should be automatically registered to the 
		/// Equipment if the function succeeds.
		/// </param>
		/// <param name="error">
		/// If nullptr is returned, this will return the reason why
		/// the function failed.
		/// </param>
		/// <returns>
		/// The Equipment object represented by the JSON, or nullptr
		/// if parsing failed.
		/// </returns>
		static SEquipmentSPtr ParseRegistration(
			const json& js, 
			std::vector<std::string> & topicsOut,
			std::string& error);
	};
}