#include "CoreDNH.h"
#include "LogSys.h"

namespace CVG
{
	CoreDNH::CoreDNH()
		:	httpServer(5700, this),
			wsServer(5701, this),
		cmdsStartup("startupcmds"),
		cmdsReset("resetcmds"),
		cmdsFatal("fatalcmds"),
		cmdsEnd("endcmds")
	{
		this->equipment = EquipmentListSPtr(new SEquipmentList());
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

	ParamCacheSPtr CoreDNH::GetDataCache()
	{
		const std::lock_guard<std::mutex> lock(this->datacacheMutex);
		return this->datacache.Clone();
	}

	void CoreDNH::Tick()
	{
		// Currently an empty placeholder.
		//
		// Regular interval logic while the DNH system is
		// running would go here.
	}

	bool CoreDNH::Register(WSConSPtr con, SEquipmentSPtr eq)
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

	bool CoreDNH::Unregister(SEquipmentSPtr eq)
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
		json jsEqList = eqLst->GetJSON();
		// The system is a special Equipment type.
		json sysEq = SEquipment::EquipmentJSONTemplate("system", "system", EQType::System, "hub");
		{
			std::lock_guard<std::mutex> lock(this->datacacheMutex);
			sysEq["params"] = this->datacache.JSONDefinitionsArray();
		}
		jsEqList.push_back(sysEq);
		//
		j["equipment"] = jsEqList;
		return j;
	}

	json CoreDNH::GenerateJSON_System()
	{
		LogSys::LogVerbose("Generating System JSON");

		json j;
		// TODO:
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
		// TODO:
		j["UNIMPLEMENTED"] = "UNIMPLEMENTED";
		return j;
	}

	void CoreDNH::Reset(const std::string& reason, bool runResetCmds)
	{
		// The reset is a very invasive process - we're 
		// going to lockdown every mutex involved until
		// the entire Reset process is over.
		//
		// We don't need connections mucking things up while
		// we're making sure the reset state is primmed and proper.
		{
			std::lock_guard<std::mutex> lockDatacache(this->datacacheMutex);
			std::lock_guard<std::mutex> lockEquipment(this->equipmentMutex);

			json jsbroad;
			jsbroad["apity"] = "reset";
			jsbroad["reason"] = reason;
			// Reset data cache
			//////////////////////////////////////////////////
			std::set<std::string> changedCaches;
			std::set<std::string> removedCaches;
			json jsCacheRm = json::array();		// data cache ids removed.
			json jsCacheVals;					// modified datacache updated values
			json jsCacheIds = json::array();	// array of id keys for jsCacheVals
			this->datacache.Reset(true, &changedCaches, &removedCaches);
			
			for (const std::string& s : removedCaches)
				jsCacheRm.push_back(s);
			
			for (const std::string& s : changedCaches)
			{
				ParamSPtr p = this->datacache.Get(s);
				if (p == nullptr)
					continue;

				jsCacheIds.push_back(s);
				jsCacheVals[s] = p->GetValueJSON();
			}
			
			jsbroad["chrmv"] = jsCacheRm;
			jsbroad["chids"] = jsCacheIds;

			// A dictionary of all the changed values, with the
			// key being the Equipment guids, and the value being
			// another diction of the paramids and value that were
			// changed. Only params with defaults will be changed.
			json jseqvals;
			// The array of all GUIDs used as keys in jseqvals
			json jsguids = json::array();

			// Reset all equipment
			//
			// This is pretty much the only place (as of writing,
			// 11/03/2021) where the CoreDNH actually manually does
			// websockets work.
			// 
			// Ideally the CoreDNH shouldn't be directly doing WS work,
			// but it would be just as invasive to deffer this to the WS
			// system to do our equipment reset - especially with the
			// interface it's allowed and how the equipmentMutex is locked
			// at the start of this reset process.
			//////////////////////////////////////////////////

			// For all equipments
			for (auto it : this->equipment->itemsByGUID)
			{
				bool any = false;
				
				// A dictionary of all Params changed.
				// keys: Param id
				// values: current value
				json jseq; 

				// For all Params
				for (ParamSPtr p : *it.second)
				{
					// Except only the params that actually have defaults
					// to reset to
					if (p->ResetToDefault() == true)
					{
						jseq[p->GetID()] = p->GetValueJSON();
						any = true;
					}
				}

				if (any == true)
				{
					jseqvals[it.first] = jseq;
					jsguids.push_back(it.first);
				}
			}
			jsbroad["eqvals"] = jseqvals;
			jsbroad["eqguids"] = jsguids;

			std::string strbroad = jsbroad.dump();
			this->equipment->Broadcast(strbroad);
		}

		// Run any execution commands in the configuration.
		//////////////////////////////////////////////////

		// We don't do this with the mutexes locked. If there are 
		// commands, it's possible they run programs that interact 
		// with the bus - and we don't want the bus mutex locked for
		// that.
		if (runResetCmds == true)
			this->cmdsReset.Execute();
	}

	void CoreDNH::Ping()
	{
		this->wsServer.Ping();
	}

	ParamSPtr CoreDNH::_RegisterDataCache(const json& js, std::string& error)
	{
		// Right now it's just a simple ParseParamJSON(), but this
		// ensures if it ever becomes more complicated that 
		// RegisterDataCache() and RegisterDataCacheArray() will
		// handle the data the same.
		return this->datacache.ParseParamJSON(js, error);
	}

	ParamSPtr CoreDNH::RegisterDataCache(const json& js, std::string& error)
	{
		std::lock_guard<std::mutex> lock(this->datacacheMutex);
		return this->_RegisterDataCache(js, error);
	}

	void CoreDNH::RegisterDataCacheArray(
		const json& jsArray,
		std::vector<std::string>& errors,
		std::vector<std::string>& successes)
	{
		std::lock_guard<std::mutex> lock(this->datacacheMutex);

		if (!jsArray.is_array())
		{
			errors.push_back("Registration for datacache was not an array.");
			return;
		}

		if (jsArray.size() == 0)
		{
			errors.push_back("Empty Param definitions array.");
			return;
		}

		for (const json& js : jsArray)
		{
			std::string error;
			ParamSPtr regParam = this->_RegisterDataCache(js, error);

			if (regParam == nullptr)
			{
				if (!error.empty())
					errors.push_back(error);
			}
			else
				successes.push_back(regParam->GetID());
		}

		if (successes.size() == 0 && errors.size())
			errors.push_back("Nothing was registered.");
	}

	void CoreDNH::ProcessConfigurationJSON(const json& js)
	{
		if (js.contains("params") && js["params"].is_array())
		{
			LogSys::Log("Parsing config Params.");
			{
				std::lock_guard<std::mutex> guard(this->datacacheMutex);
				for (const json& jsp : js["params"])
				{
					std::string error;
					ParamSPtr p = this->datacache.ParseParamJSON(jsp, error);
					if (p == nullptr && error.size() > 0)
						LogSys::Log("Error parsing Param definition entry: " + error);
				}
			}
			LogSys::Log("Finished parsing config Params.");
		}

		struct $_anon
		{
			static void DumpJSONCmdsIntoCmdCol(
				const json& jsSrc,
				const std::string& name,
				CommandsCollection& dst)
			{
				if (!jsSrc.contains(name) || !jsSrc[name].is_array())
					return;

				LogSys::Log("Parsing command collection for " + name);

				for (const json & c : jsSrc[name])
				{
					if (c.is_string())
						dst.AddCommand(c);
				}
			}
		};
		
		$_anon::DumpJSONCmdsIntoCmdCol(js, "startupcmds",	this->cmdsStartup);
		$_anon::DumpJSONCmdsIntoCmdCol(js, "resetcmds",		this->cmdsReset);
		$_anon::DumpJSONCmdsIntoCmdCol(js, "fatalcmds",		this->cmdsFatal);
		$_anon::DumpJSONCmdsIntoCmdCol(js, "endcmds",		this->cmdsEnd);
	}

	bool CoreDNH::FinalizeInitialization()
	{
		this->cmdsStartup.Execute();
		return true;
	}

	bool CoreDNH::FinalizeShutdown()
	{
		this->cmdsEnd.Execute();
		return true;
	}
}