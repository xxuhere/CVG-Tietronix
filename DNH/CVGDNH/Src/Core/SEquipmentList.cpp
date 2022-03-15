#include "SEquipmentList.h"

namespace CVG
{
	json SEquipmentList::GetJSON() const
	{
		json ret = json::array();

		for (auto it : this->itemsByGUID)
			ret.push_back(it.second->GetJSON());

		return ret;
	}

	json SEquipmentList::GetJSONSummary() const
	{
		json ret = json::array();

		for (auto it : this->itemsByGUID)
			ret.push_back(it.second->GetJSONSummary());

		return ret;
	}

	bool SEquipmentList::Register(
		WSConSPtr con, 
		SEquipmentSPtr eq, 
		const std::string& loopbackAddr)
	{
		// We only register if it's doesn't currently exist.
		//
		// It's assumed if eq isn't in itemsByGUID, con and eq won't be
		// in this->itemsBySocket.
		if (this->itemsByGUID.find(eq->GUID()) != this->itemsByGUID.end())
			return false;

		eq->SetSocket(con, loopbackAddr);

		this->itemsByGUID[eq->GUID()] = eq;
		this->itemsBySocket[con] = eq;

		return true;
	}

	bool SEquipmentList::RemoveByGUID(const std::string& guid)
	{
		auto it = this->itemsByGUID.find(guid);
		if (it == this->itemsByGUID.end())
			return false;

		// Remove the GUID map

		// Get a copy before we invalidate the iterator by erasing it.
		SEquipmentSPtr eq = it->second;
		this->itemsByGUID.erase(it);

		// And to keep them in sync, remove the socket map.
		// This is similar to the check in RemoveBySocket() which 
		// then deffers here. The overhead is ignored.
		auto itsock = this->itemsBySocket.find(eq->GetSocket());
		if (itsock != this->itemsBySocket.end())
			this->itemsBySocket.erase(itsock);

		eq->Deactivate();

		return true;
	}

	std::string SEquipmentList::RemoveBySocket(WSConSPtr con)
	{
		auto it = this->itemsBySocket.find(con);
		if (it == this->itemsBySocket.end())
			return std::string();

		// Delegate to GUID, this means one less places syncing 
		// itemsBySocket and itemsBySocket take place.
		std::string ret = it->second->GUID();
		this->RemoveByGUID(it->second->GUID());
		return ret;
	}

	void SEquipmentList::Broadcast(const std::string& message, std::set<WSConSPtr> ignore)
	{
		std::shared_ptr<WsServer::OutMessage> sendStream = 
			std::make_shared< WsServer::OutMessage>();

		*sendStream << message;

		for (auto it : this->itemsByGUID)
		{
			WSConSPtr con = it.second->GetSocket();
			if (ignore.find(con) != ignore.end())
				continue;

			con->send(sendStream);
		}
	}

	SEquipmentSPtr SEquipmentList::FindGUID(const std::string& guid)
	{
		auto it = this->itemsByGUID.find(guid);
		if (it == this->itemsByGUID.end())
			return nullptr;

		return it->second;
	}

	SEquipmentSPtr SEquipmentList::FindConnection(WSConSPtr con)
	{
		auto it = this->itemsBySocket.find(con);
		if (it == this->itemsBySocket.end())
			return nullptr;

		return it->second;
	}

	EquipmentListSPtr SEquipmentList::Clone()
	{
		SEquipmentList* pel = new SEquipmentList();
		EquipmentListSPtr ret = EquipmentListSPtr(pel);

		pel->itemsByGUID.insert(
			this->itemsByGUID.begin(), 
			this->itemsByGUID.end());

		pel->itemsBySocket.insert(
			this->itemsBySocket.begin(),
			this->itemsBySocket.end());

		return ret;
	}
}