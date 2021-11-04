#pragma once

#include <thread>
#include "BackboneHTTP.h"
#include "BackboneWS.h"
#include "Equipment.h"
#include "UtilsClasses/ParamCache.h"
#include "UtilsClasses/CommandsCollections.h"

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

		// Arbitary data tied to the server session - as opposed to
		// an equipment session.
		ParamCache datacache;

		// The mutext to access datacache, both
		// for read and write.
		std::mutex datacacheMutex;

		CommandsCollection cmdsStartup;
		CommandsCollection cmdsReset;
		CommandsCollection cmdsFatal;
		CommandsCollection cmdsEnd;

		bool isShutdown = false;

	private:
		/// <summary>
		/// Update the equipment cache.
		/// </summary>
		void CacheCurrentEquipment();

		// Implementation of RegisterDataCache without the mutex guard so
		// the logic can be reused in the *Array() version.
		ParamSPtr _RegisterDataCache(const json& js, std::string & error);

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

		// Currently there is no way to toggle the shutdown process,
		// but this is used at the condition for the background 
		// application loop.
		inline bool IsShutdown()
		{ return this->isShutdown; }

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
		/// Register a JSON Param definition.
		/// </summary>
		/// <param name="js">The Param definition.</param>
		/// <param name="error">
		/// If the function returns nullptr, this output string will explain
		/// why the parse and registration failed. If the function successfully
		/// created a Param, this output should be ignored.
		/// 
		/// If js is a element of an array filled with Param definitions that
		/// need to be registered, use RegisterDataCacheArray() for more efficient
		/// thread protections.
		/// </param>
		/// <returns>
		/// The created Param, or nullptr if there was an error.
		/// </returns>
		ParamSPtr RegisterDataCache(const json& js, std::string & error);

		/// <summary>
		/// Register an array of Param definitions.
		/// </summary>
		/// <param name="js">The JSON array of Param definitions to parse.</param>
		/// <param name="errors"> The errors found.</param>
		/// <param name="successes">
		/// The list of successful Param ids created.
		/// </param>
		/// <returns></returns>
		void RegisterDataCacheArray(
			const json& jsArray, 
			std::vector<std::string> & errors,
			std::vector<std::string> & successes);

		/// <summary>
		/// Get a copy of the data cache.
		/// 
		/// Note this is a full copy of the map so that after it's sent, we do
		/// not need to keep it locked in a mutex for writes - even if there is
		/// a risk of it becoming out of date.
		/// 
		/// The ParamSPtrs are direct references so those can stay up to date but
		/// will require their own mutexes if syncing and thread-saftey is an issue.
		/// </summary>
		/// <returns>A copy of the data cache</returns>
		ParamCacheSPtr GetDataCache();

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
		/// Reset the session.
		/// </summary>
		void Reset(const std::string & reason, bool runResetCmds = true);

		/// <summary>
		/// Pings live connections to the hub.
		/// </summary>
		void Ping();

		/// <summary>
		/// Process a JSON used as the configuration file.
		/// 
		/// This should be called before any other initialization
		/// is done to the DNH, as this may contain parameters for
		/// those other initialization processes.
		/// </summary>
		/// <param name="js">The json to configure the DNH.</param>
		void ProcessConfigurationJSON(const json& js);

		/// <summary>
		/// This should be called after other initializations when
		/// setting up the DNH, but before the DNH enters into a
		/// background application loop.
		/// </summary>
		/// <returns>True if no errors encountered.</returns>
		bool FinalizeInitialization();

		/// <summary>
		/// This should be called after the background application loop
		/// has been exited - which is assumed to be during the application's
		/// shutdown process.
		/// </summary>
		/// <returns>True if no errors encountered.</returns>
		bool FinalizeShutdown();
	};
}