#pragma once

#include <vector>
#include <map>

#include "SocketDefines.h"
#include "Types/DataType.h"
#include "Types/EqType.h"
#include "Params/Param.h"

#include <set>

namespace CVG 
{
	/// <summary>
	/// The DNH server runtime representation of Equipment.
	/// </summary>
	class Equipment
	{
	
	public:

		// And iterator for the params, to allow things to
		// traverse the Params, but also make sure the 
		// vector is unmodifyable.
		class iterator
		{
			friend Equipment;

		private:
			// We'll just borrow the actual vector iterator, but 
			// keep direct access to it protected.
			std::vector<ParamSPtr>::iterator it;

		private:
			iterator(std::vector<ParamSPtr>::iterator it);
			
		public:
			// Iterator implementation functions to allow ranged for 
			// loops. Other iterator functions are ignored.
			ParamSPtr operator *();
			void operator++();
			void operator--();
			bool operator == (const iterator& itOther) const;
			bool operator != (const iterator& itOther) const;
		};

	private:
		// The active state of the Equipment. It will only 
		// change the first time Deactivate() is called on it.
		bool active = true;

		// The name of the Equipment.
		std::string name;

		// The name of the Equipment manufacturer. This can be 
		// empty if the concept of a manufactuerer does not apply.
		// (Such as for spectator connections)
		std::string manufacturer;

		/// <summary>
		/// Specify the Equipment's purpose in a device stack.
		/// </summary>
		std::string purpose;

		// The GUID of the equipment. This value is assigned by the
		// DNH during runtime, and will be unique for ALL Equipment.
		std::string guid;

		// The hostname of the equipment. If the host wants to provide
		// its local network ID so other things can directly connect
		// to it.
		std::string hostname;

		// The type of equipment.
		EQType equipmentType;

		// The epoch timestamp of when the Equipment registered.
		//
		// This is kind of misnomer, it will actually be the moment
		// the Equipment object was created on the server - but for 
		// all intents and purposes, they should be treated as the 
		// same.
		unsigned long long timestampRegistered;

		// The reflective parameters of the Equipment.
		std::vector<ParamSPtr> params;

		// The mutex that must be locked whenever topics is accessed
		// for read or write.
		std::mutex topicsMutex;

		// The set of topics the Equipment is connected to.
		std::set<std::string> topics;

		// Extra misc data. Since we don't know what it is, we'll
		// leave it as JSON since that's a good format for holding
		// arbitrary typed data hierarchies.
		json clientData;

		// The Equipment's realtime network connection to the DNH server.
		WSConSPtr socket;

	public:
		/// <summary>
		/// Create the basic JSON object for an Equipment definition.
		/// 
		/// Some things that aren't actually Equipment may wish to 
		/// defined themselves in JSON the same way Equipments do.
		/// </summary>
		/// <param name="name">The Equipment type.</param>
		/// <param name="guid">The Equipment guid.</param>
		/// <param name="type">The Equipment type.</param>
		/// <param name="purpose">The Equipment purpose.</param>
		/// <returns>The barebones Equipment definition.</returns>
		static json EquipmentJSONTemplate(
			const std::string& name,
			const std::string& guid,
			EQType eqTy,
			const std::string& purpose);

	public:

		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="name">The name of the Equipment.</param>
		/// <param name="manufacturer">Optional. The manufacturer of the equipment.</param>
		/// <param name="type">The equipment type.</param>
		/// <param name="params">The equipment's reflective parameters.</param>
		/// <param name="clientData">
		/// Extra arbitrary data tied to the Equipment's representation.
		/// </param>
		Equipment(
			const std::string & name, 
			const std::string & manufacturer,
			const std::string & purpose,
			const std::string& hostname,
			EQType type,
			std::vector<ParamSPtr> params,
			json clientData);

		std::string GUID() const 
		{ return this->guid; }

		std::string Purpose() const 
		{ return this->purpose; }

		inline std::string TypeStr() const
		{ return ConvertToString(this->equipmentType); };

		inline EQType Type() const 
		{ return this->equipmentType; }

		inline std::string Name()const
		{ return this->name; }

		inline unsigned long long TimestampRegistered() const
		{ return this->timestampRegistered; }

		/// <summary>
		/// Parameter iterator to allow iteration and use of 
		/// ranged-for loops.
		/// </summary>
		/// <returns>The starting iterator for contained Params.</returns>
		iterator begin();

		/// <summary>
		/// Parameter iterator to allow iteration and use of 
		/// ranged-for loops.
		/// </summary>
		/// <returns>The ending iterator for contained Params.</returns>
		iterator end();

		/// <summary>
		/// Flag the Equipment as unregistrered.
		/// 
		/// When an Equipment is removed from the system, this may
		/// still contain (smart pointer) references to it. By 
		/// Deactivating the Equipment, things can query to see if
		/// the references are still viable.
		/// 
		/// This is a one-way process. Anything deactivated can not
		/// be un-deactivated.
		/// </summary>
		/// <returns>
		/// True if deactivated. False if the Equipment was already
		/// deactivated.
		/// </returns>
		bool Deactivate();

		inline bool IsActive() const 
		{ return this->active; }

		/// <summary>
		/// Checks if the Equipment is subscribed to a topic.
		/// </summary>
		/// <param name="topic">The topic to check.</param>
		/// <returns>True if the Equipment is subscribed to the topic. Else, false.</returns>
		bool IsSubscribed(const std::string & topic);

		/// <summary>
		/// Subscribes the Equipment to a set of topics.
		/// 
		/// While there is a singular version of Subscribe(), this is preffered
		/// over individual Subscribe() calls if there are multiple topics because
		/// it does not thrash the topics mutex.
		/// </summary>
		/// <param name="topics">The topics to subscribe to.</param>
		/// <returns>
		/// True if the Equipment was successfully subscribed to any
		/// topics. Else, false.
		/// </returns>
		bool Subscribe(const std::vector<std::string>& topics);

		/// <summary>
		/// Unsubscribes the Equipment from a single topic.
		/// </summary>
		/// <param name="topic">The topic to subscribe to.</param>
		/// <returns>True if the subscription was successful. Else, false.</returns>
		bool Subscribe(const std::string& topic);

		/// <summary>
		/// Unsubscribes the Equipment from a set of topics.
		/// 
		/// Performs a batch subscription
		/// </summary>
		/// <param name="topics">The topic to unsubscribe to.</param>
		/// <returns>True if the unsubscription was successful. Else, false.</returns>
		bool Unsubscribe(const std::vector<std::string>& topics);

		/// <summary>
		/// Convenience function to unsubscribe from a single topic.
		/// 
		/// If multiple topics need to be unsubscribed in batch, use the
		/// batch function, as it manages the thread mutex better for
		/// multiple operations.
		/// </summary>
		/// <param name="topic">The topic to unsubscribe from.</param>
		/// <returns>True if the unsubscription was successful. Else, false.</returns>
		bool Unsubscribe(const std::string& topic);

		/// <summary>
		/// Send a topic message to the Equipment.
		/// 
		/// The request will be ignored if the Equipment isn't subscribed
		/// to the topic.
		/// </summary>
		/// <param name="topic">The topic the message is for.</param>
		/// <param name="payload">The topic data to send. This should be a topic API message.</param>
		/// <returns>
		/// If true, the message was sent. Else if false, the Equipment
		/// was not subscribed to the topic.
		/// </returns>
		bool TryTopicSend(const std::string& topic, const std::string& payload);

		/// <summary>
		/// Get an in-depth description of the Equipment as a JSON
		/// representation.
		/// 
		/// This includes the introspection and current value of the 
		/// parameters.
		/// </summary>
		/// <returns>
		/// Details, and the current state, of the Equipment as a 
		/// JSON object.
		/// </returns>
		json GetJSON() const;

		/// <summary>
		/// Get an short description of the Equipment as a JSON represention.
		/// </summary>
		/// <returns>The equipment's name and GUID as a JSON object.</returns>
		json GetJSONSummary() const;

		/// <summary>
		/// Set the WebSocket for the Equipment. This is only allowed
		/// to be set once.
		/// </summary>
		/// <param name="s">
		/// The WebSocket connection that is networked with the Equipment.
		/// </param>
		/// <returns>
		/// True if the network connection was successfuly set. 
		/// False if the socket was already set.
		/// </returns>
		bool SetSocket(WSConSPtr s);

		inline WSConSPtr GetSocket() 
		{ return this->socket; }

		/// <summary>
		/// Find a parameter in the Equipment.
		/// </summary>
		/// <param name="id">The id of the Parameter.</param>
		/// <returns>
		/// A reference to the Param in the Equipment.
		/// nullptr will be found if a Param with a matching id was 
		/// not found.
		/// </returns>
		ParamSPtr GetParam(const std::string& id);
	};

	typedef std::shared_ptr<Equipment> EquipmentSPtr;

	class EquipmentList;
	typedef std::shared_ptr<EquipmentList> EquipmentListSPtr;

	/// <summary>
	/// A Collection of equipment.
	/// </summary>
	class EquipmentList
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
		std::map<std::string, EquipmentSPtr> itemsByGUID;

		/// <summary>
		/// All the Equipment in the list, organized by their sockets.
		/// 
		/// </summary>
		/// <remark>
		/// </remark>
		std::map<WSConSPtr, EquipmentSPtr> itemsBySocket;

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
		/// <returns>
		/// A JSON array.
		/// </returns>
		bool Register(WSConSPtr con, EquipmentSPtr eq);

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
		EquipmentSPtr FindGUID(const std::string& guid);

		/// <summary>
		/// Find an Equipment with a matching WebSocket connection.
		/// </summary>
		/// <param name="con">Th WebSocket connection to search for.</param>
		/// <returns>The Equipment with the matching connection. Or nullptr if none was found.</returns>
		EquipmentSPtr FindConnection(WSConSPtr con);

		/// <summary>
		/// Create a copy of the EquipmentList.
		/// </summary>
		/// <returns>A copy of the EquipmentList.</returns>
		EquipmentListSPtr Clone();
	};

}

