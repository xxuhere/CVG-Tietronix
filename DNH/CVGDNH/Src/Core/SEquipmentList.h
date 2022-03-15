#pragma once

#include <vector>
#include <map>
#include <set>
#include "SocketDefines.h"
#include "SEquipment.h"

namespace CVG 
{
	class SEquipmentList;
	typedef std::shared_ptr<SEquipmentList> EquipmentListSPtr;

	/// <summary>
	/// A Collection of server-side equipments.
	/// </summary>
	class SEquipmentList
	{
	public:
		// The values itemsByGUID and itemsBySocket really 
		// should be privately encapsulated, but for now
		// they're left public so they can easily be used
		// in range-based for loops.

		/// <summary>
		/// All the Equipment in the list, organized by their GUIDs.
		/// The Equipment values should match exactly with itemsBySocket.
		/// </summary>
		std::map<std::string, SEquipmentSPtr> itemsByGUID;

		/// <summary>
		/// All the Equipment in the list, organized by their sockets.
		/// 
		/// </summary>
		/// <remark>
		/// </remark>
		std::map<WSConSPtr, SEquipmentSPtr> itemsBySocket;

		/// <summary>
		/// Generate an in-depth representation of all the Equipment
		/// contained in the EquipmentList, in the form of a JSON
		/// array.
		/// </summary>
		/// <returns></returns>
		json GetJSON() const;

		/// <summary>
		/// Generate a summary of all the Equipment contained in the 
		/// EquipmentList, in the form of a JSON array.
		/// </summary>
		/// <returns>
		/// A JSON array.
		/// </returns>
		json GetJSONSummary() const;

		/// <summary>
		/// Register an Equipment and tie it to a WebSocket connection.
		/// </summary>
		/// <param name="con">The connection the Equipment is tied to.</param>
		/// <param name="eq">The Equipment to register.</param>
		/// <param name="loopbackAddr">The address to use if con is a loopback.</param>
		/// <returns>
		/// A JSON array.
		/// </returns>
		bool Register(WSConSPtr con, SEquipmentSPtr eq, const std::string& loopbackAddr);

		/// <summary>
		/// Remove an Equipment from the list that is 
		/// tied to a GUID.
		/// </summary>
		/// <param name="guid">The GUID to remove.</param>
		/// <returns>
		/// True if successfully removed. False if an Equipment with
		/// the specified GUID was not found.
		/// </returns>
		bool RemoveByGUID(const std::string& guid);

		/// <summary>
		/// Remove an Equipment from the list that is
		/// tied to a WebSocket connection.
		/// </summary>
		/// <param name="con">The connect to remove.</param>
		/// <returns>
		/// If successful, the GUID of the Equipment removed.
		/// Else, an empty string.
		/// </returns>
		std::string RemoveBySocket(WSConSPtr con);

		/// <summary>
		/// Broadcast a string message to all connections in the list.
		/// </summary>
		/// <param name="message">The message to broadcast.</param>
		/// <param name="ignore">The set of connections to ignore broadcasting to.</param>
		void Broadcast(const std::string& message, std::set<WSConSPtr> ignore);

		/// <summary>
		/// Broadcast a message to everybody.
		/// </summary>
		/// <param name="message">The data to broadcast. This is expected to be 
		/// a JSON formatted string.</param>
		inline void Broadcast(const std::string& message)
		{
			std::set<WSConSPtr> _;
			this->Broadcast(message, _);
		}

		/// <summary>
		/// Broadcast a message to everybody except for one connection.
		/// 
		/// This is often used when a broadcast is made to reflect a change in the system.
		/// The requestor will usually get a different response than what's broadcasted
		/// to everybody else.
		/// </summary>
		/// <param name="message">The data to broadcast. This is expected to be
		/// a JSON formatted string.</param>
		/// <param name="singleIgnore">The connection to exclude broadcasting from.</param>
		inline void Broadcast(const std::string& message, WSConSPtr singleIgnore)
		{
			std::set<WSConSPtr> ign;
			ign.insert(singleIgnore);
			this->Broadcast(message, ign);
		}

		/// <summary>
		/// Find an Equipment with a matching GUID.
		/// </summary>
		/// <param name="guid">The GUID to search for.</param>
		/// <returns>The Equipment with the matching GUID. Or nullptr if none was found.</returns>
		SEquipmentSPtr FindGUID(const std::string& guid);

		/// <summary>
		/// Find an Equipment with a matching WebSocket connection.
		/// </summary>
		/// <param name="con">Th WebSocket connection to search for.</param>
		/// <returns>The Equipment with the matching connection. Or nullptr if none was found.</returns>
		SEquipmentSPtr FindConnection(WSConSPtr con);

		/// <summary>
		/// Create a copy of the EquipmentList.
		/// </summary>
		/// <returns>A copy of the EquipmentList.</returns>
		EquipmentListSPtr Clone();
	};
}