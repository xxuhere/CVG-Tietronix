#pragma once

#include "BackboneHTTP.h"
#include "BackboneWS.h"
#include "Equipment.h"
#include <thread>

namespace CVG
{
	/// <summary>
	/// The DNH system.
	/// </summary>
	class CoreDNH
	{
	private:
		// The HTTP server component.
		BackboneHTTP httpServer;

		// The WebSocket server component.
		BackboneWS wsServer;

		// The up-to-date registry of Equipment.
		EquipmentListSPtr equipment;

		// Tracks if equipment and equipmentCached
		// contain different elements.
		bool equipmentCacheDirty = true;

		// A cached version of equipment. There is a risk that
		// this version may be slightly outdated, but it can be
		// read by any thread without locking the equipment mutex.
		//
		// It will be updated to match equipment, when dirty, as 
		// reasonably often as possible.
		EquipmentListSPtr equipmentCached;

		// The mutex to access equipment, both 
		// for read and write.
		std::mutex equipmentMutex;

	private:
		/// <summary>
		/// Update the equipment cache.
		/// </summary>
		void CacheCurrentEquipment();

	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		CoreDNH();

		/// <summary>
		/// Start all servers.
		/// </summary>
		/// <returns>True if successful. Else, false.</returns>
		bool StartNetwork();

		/// <summary>
		/// Stop all servers.
		/// </summary>
		/// <returns>True if successful. Else, false.</returns>
		bool StopNetwork();

		/// <summary>
		/// Wait for all servers to come to a complete stop.
		/// </summary>
		/// <returns></returns>
		bool WaitForNetworkStop();

		/// <summary>
		/// Check if all servers are running.
		/// </summary>
		bool IsFullyRunning();

		/// <summary>
		/// Check if any servers are running.
		/// </summary>
		bool IsAnyRunning();

		/// <summary>
		/// Notify the system that the equipment cache not longer
		/// matches the current equipment list. 
		/// 
		/// In order to reset the dirty flag, CacheCurrentEquipment()
		/// should be called.
		/// </summary>
		void FlagEquipmentCacheDirty();

		/// <summary>
		/// Get the cached list of Equipment.
		/// </summary>
		/// <returns>The cached list of registered Equipment.</returns>
		EquipmentListSPtr GetEquipmentCache();

		/// <summary>
		/// Register an Equipment with the DNH.
		/// </summary>
		/// <param name="con">The connection to tie to the Equipment.</param>
		/// <param name="eq">The Equipment to register.</param>
		/// <returns>True if successfully registered. Else, false.</returns>
		bool Register(WSConSPtr con, EquipmentSPtr eq);

		/// <summary>
		/// Unregister an Equipment.
		/// </summary>
		/// <param name="eq">The Equipment to unregister.</param>
		/// <returns>True if succesfully unregistered. Else, false.</returns>
		bool Unregister(EquipmentSPtr eq);

		/// <summary>
		/// Unregister an Equipment tied to a GUID.
		/// </summary>
		/// <param name="guid">The GUID to remove.</param>
		/// <returns>True if successfully unregistered. Else, false.</returns>
		bool Unregister(std::string guid);

		/// <summary>
		/// Unregister an Equipment tied to a WebSocket connection.
		/// </summary>
		/// <param name="con">The connection to remove.</param>
		/// <returns>True is successfully unregistered. Else, false.</returns>
		std::string Unregister(WSConSPtr con);

		/// <summary>
		/// A function that is run repeatedly while the
		/// system is active.
		/// </summary>
		void Tick();

		/// <summary>
		/// Generate the JSON value for /equipment requests.
		/// </summary>
		/// <returns>The API response for /equipment.</returns>
		json GenerateJSON_Equipment();

		/// <summary>
		/// Generate the JSON value for /system requests.
		/// </summary>
		/// <returns>The API response for /system.</returns>
		json GenerateJSON_System();

		/// <summary>
		/// Generate the JSON value for /status requests.
		/// </summary>
		/// <returns>The API response for /status.</returns>
		json GenerateJSON_Status();

		/// <summary>
		/// Broadcasts a ping to all connected users. This includes
		/// unregistered connections.
		/// </summary>
		void Ping();
	};
}