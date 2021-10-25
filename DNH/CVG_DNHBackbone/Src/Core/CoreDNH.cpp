#include "CoreDNH.h"
#include "LogSys.h"

namespace CVG
{
	CoreDNH::CoreDNH()
		:	httpServer(5000, this),
			wsServer(5001, this)
	{
		this->equipment = EquipmentListSPtr(new EquipmentList());
	}

	void CoreDNH::CacheCurrentEquipment()
	{
		if (this->equipmentCacheDirty == true)
		{
			this->equipmentMutex.lock();
			{
				this->equipmentCached = this->equipment->Clone();
				this->equipmentCacheDirty = false;
			}
			this->equipmentMutex.unlock();
		}
	}

	void CoreDNH::FlagEquipmentCacheDirty()
	{
		this->equipmentCacheDirty = true;
	}

	bool CoreDNH::StartNetwork()
	{
		bool ret = true;
		ret &= this->httpServer.StartServer();
		ret &= this->wsServer.StartServer();
		return ret;
	}

	bool CoreDNH::StopNetwork()
	{
		// TODO:
		return false;
	}

	bool CoreDNH::WaitForNetworkStop()
	{
		// !TODO:
		return false;
	}

	bool CoreDNH::IsFullyRunning()
	{
		return
			this->httpServer.IsRunning() &&
			this->wsServer.IsRunning();
	}

	bool CoreDNH::IsAnyRunning()
	{
		return 
			this->httpServer.IsRunning() || 
			this->wsServer.IsRunning();
	}

	EquipmentListSPtr CoreDNH::GetEquipmentCache()
	{
		// Update the cache if dirty
		this->CacheCurrentEquipment();

		return this->equipmentCached;
	}

	void CoreDNH::Tick()
	{
		// Currently an empty placeholder.
		//
		// Regular interval logic while the DNH system is
		// running would go here.
	}

	bool CoreDNH::Register(WSConSPtr con, EquipmentSPtr eq)
	{
		bool ret = false;
		this->equipmentMutex.lock();

		if (this->equipment->Register(con, eq))
		{
			this->FlagEquipmentCacheDirty();
			ret = true;
		}

		this->equipmentMutex.unlock();
		return ret;
	}

	bool CoreDNH::Unregister(EquipmentSPtr eq)
	{
		return this->Unregister(eq->GUID());
	}

	bool CoreDNH::Unregister(std::string guid)
	{
		bool ret = false;
		this->equipmentMutex.lock();

		if (this->equipment->RemoveByGUID(guid))
		{
			this->FlagEquipmentCacheDirty();
			ret = true;
		}

		this->equipmentMutex.unlock();
		return ret;
	}

	std::string CoreDNH::Unregister(WSConSPtr con)
	{
		const std::lock_guard<std::mutex> lock(this->equipmentMutex);

		std::string remret = this->equipment->RemoveBySocket(con);
		if (remret.size() > 0)
			this->FlagEquipmentCacheDirty();

		return remret;
	}

	json CoreDNH::GenerateJSON_Equipment()
	{
		LogSys::LogVerbose("Generating Equipment JSON");

		json j;
		j["apity"] = "equipment";

		EquipmentListSPtr eqLst = this->GetEquipmentCache();
		j["equipment"] = eqLst->GetJSON();
		return j;
	}

	json CoreDNH::GenerateJSON_System()
	{
		LogSys::LogVerbose("Generating System JSON");

		json j;
		j["apity"] = "system";
		j["mac"] = "TODO";
		j["user"] = boost::asio::ip::host_name();
		j["os"] = "TODO";
		j["osbuild"] = "TODO";
		j["osver"] = "TODO";
		j["ramtotal"] = "TODO";
		j["ramfree"] = "TODO";
		j["storetotal"] = "TODO";
		j["storefree"] = "TODO";

		// Fill in the Equipment summary, we only send an abridged
		// version of the Equipment for the 'system' return.
		EquipmentListSPtr eqLst = this->GetEquipmentCache();
		json jEq = json::array();
		for (auto it : eqLst->itemsByGUID)
			jEq.push_back(it.second->GetJSONSummary());

		j["equipment"] = jEq;
		return j;
	}

	json CoreDNH::GenerateJSON_Status()
	{
		LogSys::LogVerbose("Generating Status JSON");

		json j;
		j["apity"] = "status";
		j["UNIMPLEMENTED"] = "UNIMPLEMENTED";
		return j;
	}

	void CoreDNH::Ping()
	{
		this->wsServer.Ping();
	}
}