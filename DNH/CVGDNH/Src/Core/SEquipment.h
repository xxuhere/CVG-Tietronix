#pragma once

#include <vector>
#include <map>

#include "SocketDefines.h"

#include "CVGData/Src/Equipment.h"

#include <set>

namespace CVG 
{
	/// <summary>
	/// The DNH server runtime representation of Equipment.
	/// </summary>
	class SEquipment : public Equipment
	{
	private:
		// The active state of the Equipment. It will only 
		// change the first time Deactivate() is called on it.
		bool active = true;

		// The epoch timestamp of when the Equipment registered.
		//
		// This is kind of misnomer, it will actually be the moment
		// the Equipment object was created on the server - but for 
		// all intents and purposes, they should be treated as the 
		// same.
		unsigned long long timestampRegistered;

		// The mutex that must be locked whenever topics is accessed
		// for read or write.
		std::mutex topicsMutex;

		// The set of topics the Equipment is connected to.
		std::set<std::string> topics;

		// The Equipment's realtime network connection to the DNH server.
		WSConSPtr socket;

	public:
		

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
		SEquipment(
			const std::string & name, 
			const std::string & manufacturer,
			const std::string & purpose,
			const std::string& hostname,
			EQType type,
			std::vector<ParamSPtr> params,
			json clientData);

		inline unsigned long long TimestampRegistered() const
		{ return this->timestampRegistered; }

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
		/// <param name="newTopics">The topics to subscribe to.</param>
		/// <returns>
		/// True if the Equipment was successfully subscribed to any
		/// topics. Else, false.
		/// </returns>
		bool Subscribe(const std::vector<std::string>& newTopics);

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
		/// <param name="remTopics">The topic to unsubscribe to.</param>
		/// <returns>True if the unsubscription was successful. Else, false.</returns>
		bool Unsubscribe(const std::vector<std::string>& remTopics);

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
		/// Get a copy of what the Equipment is subscribed to.
		/// </summary>
		/// <returns>The list of topics the Equipment is subscribed to.</returns>
		std::vector<std::string> Subscriptions();

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

		json GetJSON() const override;

		json GetJSONSummary() const override;

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
	};

	typedef std::shared_ptr<SEquipment> SEquipmentSPtr;
}

