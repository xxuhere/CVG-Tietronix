#include "BackboneWS.h"
#include "CoreDNH.h"
#include "LogSys.h"
#include "ResponseUtils.h"
#include "CVGData/Src/Params/ParamUtils.h"
#include "CVGData/Src/ParseUtils.h"

namespace CVG
{
	/// <summary>
	/// A class that can branch between being an Equipment or a
	/// ParamCacheSPtr but 
	/// </summary>
	class VirtualEquipment
	{
	public:
		// At most, 1 of these implementation pointers should be set.

		/// <summary>
		/// If set, the VirtualEquipment redirects to an actual equipment.
		/// </summary>
		SEquipmentSPtr eq;

		/// <summary>
		/// If set, the virtual Equipment redirects to a cache, assumed to
		/// by the DNH system's datacache.
		/// </summary>
		ParamCacheSPtr cache;

	public:
		bool IsValid()
		{
			// Is any implementation set?
			return eq != nullptr || cache != nullptr;
		}

		void SetEquipment(SEquipmentSPtr e)
		{
			this->eq = e;
			this->cache = nullptr;
		}

		void SetCache(ParamCacheSPtr c)
		{
			this->eq = nullptr;
			this->cache = c;
		}

		std::string GUID()
		{
			if (eq != nullptr)
				return eq->GUID();

			if (cache != nullptr)
			{
				//Assumed to be the datacache
				return "system";
			}
			return std::string();
		}

		ParamSPtr GetParam(const std::string& id)
		{
			if (eq != nullptr)
				return eq->GetParam(id);
			if (cache != nullptr)
				return cache->Get(id);

			return nullptr;
		}
	};

	BackboneWS::BackboneWS(int port, CoreDNH* coreSys)
	{
		this->coreSys = coreSys;
		server.config.port = port;

		auto& wsep = server.endpoint["^/realtime/?$"];

		// Deffer WebSocket endpoint handlers to our callback
		// functions.
		wsep.on_message =
			[this](
				std::shared_ptr<WsServer::Connection> connection,
				std::shared_ptr<WsServer::InMessage> message)
			{
				this->HandleWS_Realtime_Message(connection, message);
			};

		wsep.on_open =
			[this](std::shared_ptr<WsServer::Connection> connection)
			{
				this->HandleWS_Realtime_Open(connection);
			};

		wsep.on_close =
			[this](
				std::shared_ptr<WsServer::Connection> connection,
				int status,
				const std::string& reason)
			{
				this->HandleWS_Realtime_Close(connection, status, reason);
			};

		wsep.on_error =
			[this](
				std::shared_ptr<WsServer::Connection> connection,
				const SimpleWeb::error_code& ec)
		{
			this->HandleWS_Realtime_Error(connection, ec);
		};
	}

	bool BackboneWS::StartServer()
	{
		this->server_thread = 
			std::thread([this]() 
			{
				// Start WS-server
				server.start();
			});

		return false;
	}

	bool BackboneWS::ShutdownServer()
	{
		// TODO:

		this->server.stop();
		server_thread.join();
		return false;
	}

	bool BackboneWS::IsRunning()
	{
		// TODO:
		return false;
	}

	void BackboneWS::HandleWS_Realtime_Message(
		std::shared_ptr<WsServer::Connection> connection,
		std::shared_ptr<WsServer::InMessage> message)
	{
		std::string strmsg = message->string();

		json js;
		try
		{
			js = json::parse(strmsg);
		}
		catch (const json::parse_error & ex)
		{
			SendError(connection, ErrorTy::Error, "Request is not valid JSON: " + std::string(ex.what()), "", "");
			LogSys::Log("Rejected non-JSON request.");
			return;
		}
		catch (const std::exception& ex)
		{
			LogSys::Log(std::string("UNKNOWN PARSE EXCEPTION!: ") + ex.what());
			return;
		}

		if (js.is_object() == false)
		{
			SendError(connection, ErrorTy::Error, "Invalid request object.", "", "");
			return;
		}

		// Part of the message scheme is that messages can have a postage value that's sent
		// back, so requestors will be able to explicitly map requests to responses.
		std::string postage = ResponseUtils::ExtractPostage(js);
		if (!js.contains("apity") || !js["apity"].is_string())
		{
			SendError(connection, ErrorTy::Error, "Request missing apity member.", "", postage);
			return;
		}

		std::string apity = js["apity"];

		bool unregistered;
		{ // Scope for lock guard
			const std::lock_guard<std::mutex> lock(this->unregisteredMutex);
			unregistered = this->unregistered.find(connection) != this->unregistered.end();
		}

		// REGISTRATION
		//////////////////////////////////////////////////
		if (apity == "register")
		{
			if (!unregistered)
			{
				SendError(connection, ErrorTy::Error, "Attempting to re-register. Equipment can only be registered once.", "", postage);
				return;
			}
			this->HandleRTAPI_Register(connection, js, "");
			return;
		}

		if (unregistered)
		{
			SendError(connection, ErrorTy::Error, "Connection must be registered to submit request.", "", postage);
			return;
		}

		if (apity == "valset")
			this->HandleRTAPI_ValSet(connection, js, postage);
		else if (apity == "valget")
			this->HandleRTAPI_ValGet(connection, js, postage);
		else if (apity == "system")
			this->HandleRTAPI_System(connection, js, postage);
		else if (apity == "equipment")
			this->HandleRTAPI_Equipment(connection, js, postage);
		else if (apity == "status")
			this->HandleRTAPI_Status(connection, js, postage);
		else if (apity == "arm")
			this->HandleRTAPI_Arm(connection, js, postage);
		else if (apity == "disarm")
			this->HandleRTAPI_Disarm(connection, js, postage);
		else if (apity == "purpose")
			this->HandleRTAPI_Purpose(connection, js, postage);
		else if (apity == "subscribe")
			this->HandleRTAPI_Sub(connection, js, postage);
		else if (apity == "publish")
			this->HandleRTAPI_Publish(connection, js, postage);
		else if(apity == "datacache")
			this->HandleRTAPI_Datacache(connection, js, postage);
		else
			SendError(connection, ErrorTy::Error, "Unknown apity.", apity, postage);
	}

	void BackboneWS::HandleWS_Realtime_Open(
		std::shared_ptr<WsServer::Connection> connection)
	{
		// Track that a new connection exists that isn't 
		// registered yet.
		{
			const std::lock_guard<std::mutex> lock(this->unregisteredMutex);
			this->unregistered.insert(connection);
		}
	}

	void BackboneWS::HandleWS_Realtime_Close(
		std::shared_ptr<WsServer::Connection> connection,
		int status,
		const std::string& reason)
	{
		RemoveConnection(connection, "Client requested close.");
	}

	void BackboneWS::HandleWS_Realtime_Error(
		std::shared_ptr<WsServer::Connection> connection,
		const SimpleWeb::error_code& ec)
	{
		// TODO: More formal error logging
		LogSys::Log("Detected unexpected disconnect with connection");
		RemoveConnection(connection, "Unexpected error.");
	}

	void BackboneWS::RemoveConnection(WSConSPtr con, const std::string & reason)
	{
		LogSys::Log("Removing connection : " + reason);
		con->send_close(0, reason);

		// It will either be in unregistered or coreSys, but it's
		// simple enough to just do both without checking.
		{
			const std::lock_guard<std::mutex> lock(this->unregisteredMutex);
			this->unregistered.erase(con);
		}
		std::string remguid = this->coreSys->Unregister(con);
		// If it was registered, we need to send a broadcast
		// to let others know it's no longer a part of the system.
		if (remguid.size() > 0)
		{

			json jsNotif;
			jsNotif["apity"] = "changedroster";
			jsNotif["change"] = "rem";
			jsNotif["guid"] = remguid;

			EquipmentListSPtr equips = this->coreSys->GetEquipmentCache();
			equips->Broadcast(jsNotif.dump());
		}
	}

	std::string BackboneWS::GenerateJSONError(
		ErrorTy error, 
		const std::string& reason, 
		const std::string& request,
		const std::string& postage)
	{
		json js;
		js["apity"] = "error";
		js["reason"] = reason;

		ResponseUtils::ApplyRequest(js, request);
		ResponseUtils::ApplyPostage(js, postage);

		// While the error is intended to be sent to the recipient, we'll
		// also post the issue in the server log.
		std::stringstream sstrm;
		sstrm << "Error of type " << ConvertToString(error) << ":" << reason;
		if (request.size() > 0)
			sstrm << " - Request: " << request;
		sstrm << std::endl;
		LogSys::Log(sstrm.str());

		return js.dump(4);
	}

	void BackboneWS::SendError(
		WSConSPtr con,
		ErrorTy error,
		const std::string& reason,
		const std::string& request,
		const std::string& postage)
	{
		std::string err = GenerateJSONError(ErrorTy::Error, reason, request, postage);
		SendString(con, err);
	}

	void BackboneWS::SendString(WSConSPtr con, const std::string& payload)
	{
		std::shared_ptr<WsServer::OutMessage> strm =
			std::make_shared< WsServer::OutMessage>();

		*strm << payload;
		con->send(strm);
	}

	void BackboneWS::SendJSON(WSConSPtr con, const json& js, int indent)
	{
		SendString(con, js.dump(indent));
	}

	SEquipmentSPtr BackboneWS::ParseRegistration(
		const json& js,
		std::vector<std::string> & topicsOut,
		std::string& error)
	{
		// GENERAL MEMBERS
		//
		//////////////////////////////////////////////////

		const static std::string _RESERVED_system = ConvertToString(EQType::System);
		std::string reqtype;
		std::string reqname;
		std::string reqpurpose;
		std::string reqmanu;
		std::string reqhostname;
		std::string _guid; // Not used, because not assigned yet.

		// All things Equipment related (as opposed to SEquipment) will rely on 
		// Equipment::ParseEquipmentFields() to extract the relevant information to
		// make sure the parsing logic is unified. Everything else SEquipment 
		// will be parsed in this function on top of that.
		const json * pjsParams;
		Equipment::ParseEquipmentFields(
			js, 
			_guid, 
			reqmanu, 
			reqname, 
			reqtype, 
			reqtype, 
			reqhostname, 
			&pjsParams);

		if (reqtype.empty())
		{
			error = "Registration is missing equipment type.";
			return nullptr;
		}

		if (reqtype == _RESERVED_system)
		{
			error = std::string("Equipment type ") + _RESERVED_system + " cannot be registered; it is a reserved type.";
			return nullptr;
		}

		EQType eqtype = ConvertToEqType(reqtype);

		if (reqname.empty())
		{
			error = "Registration is missing equipment name.";
			return nullptr;
		}

		// PARAMETERS
		//
		//////////////////////////////////////////////////
		
		// The extracted parameters reified from the JASON.
		std::vector<ParamSPtr> params;

		// Check if we need to give an error. We'll have to redundantly
		// check js["params"] outside of ParseEquipmentFields.
		if(pjsParams == nullptr)
		{
			if(js.contains("params") && !js["params"].is_array())
			{
				error = "Registration params must be an array of parameter definitions.";
				return nullptr;
			}
		}
		else if (pjsParams != nullptr)
		{
			std::string paramErr;
			for (const json& itp : *pjsParams)
			{
				ParamSPtr p = ParamUtils::Parse(itp, paramErr);
				if (p == nullptr)
				{
					error = paramErr;
					return nullptr;
				}
				params.push_back(p);
			}
		}

		// EXTRACT CLIENT DATA
		//
		//////////////////////////////////////////////////

		// Everything else in the object we didn't look at and convert 
		// to a C/C++ representation gets stored as custom client data.
		json clientData = json::object();
		Equipment::ExtractClientData(clientData, js);

		// FINALIZATION
		//
		//////////////////////////////////////////////////

		SEquipment* eq = new SEquipment(reqname, reqmanu, reqpurpose, reqhostname, eqtype, params, clientData);
		return SEquipmentSPtr(eq);
	}

	void BackboneWS::HandleRTAPI_Register(WSConSPtr con, const json& js, const std::string& postage)
	{
		std::vector<std::string> topics;
		std::string parseErr;
		SEquipmentSPtr eq = ParseRegistration(js, topics, parseErr);
		if (eq == nullptr)
		{
			SendError(con, ErrorTy::Error, parseErr, "", postage);
			return;
		}
		{ // Scope for lock guard
			const std::lock_guard<std::mutex> lock(this->unregisteredMutex);

			if (!this->coreSys->Register(con, eq))
			{
				// Most likely this indicates state corruption, because there 
				// is already a re-register check directly above.
				SendError(con, ErrorTy::Error, "Could not register for unknown reason. Server state corrupted, or attempting to re-register?", "register", postage);
				return;
			}
			this->unregistered.erase(con);
		}

		// Respond to the client that the registration is successful.
		json jsReps;
		jsReps["apity"] = "register";
		jsReps["status"] = "success";
		jsReps["guid"] = eq->GUID();
		SendJSON(con, jsReps);

		// Respond to everyone else reporting the new member.
		json jsNotif;
		jsNotif["apity"] = "changedroster";
		jsNotif["change"] = "add";
		jsNotif["guid"] = eq->GUID();
		jsNotif["name"] = eq->Name();
		jsNotif["type"] = eq->TypeStr();
		EquipmentListSPtr eqlst = this->coreSys->GetEquipmentCache();
		eqlst->Broadcast(jsNotif.dump(), con);

		LogSys::Log("Registered Equipment with GUID " + eq->GUID());
	}

	void BackboneWS::HandleRTAPI_System(
		WSConSPtr con, 
		const json& js, 
		const std::string& postage)
	{
		json j = this->coreSys->GenerateJSON_Equipment();
		ResponseUtils::ApplyPostage(j, postage);

		SendJSON(con, j, 4);
	}

	void BackboneWS::HandleRTAPI_Equipment(
		WSConSPtr con, 
		const json& js, 
		const std::string& postage)
	{
		json j = this->coreSys->GenerateJSON_Equipment();
		ResponseUtils::ApplyPostage(j, postage);

		SendJSON(con, j, 4);
	}

	void BackboneWS::HandleRTAPI_Status(
		WSConSPtr con, 
		const json& js, 
		const std::string& postage)
	{
		json j = this->coreSys->GenerateJSON_Status();
		ResponseUtils::ApplyPostage(j, postage);

		SendJSON(con, j, 4);
	}

	void BackboneWS::HandleRTAPI_ValGet(
		WSConSPtr con, 
		const json& js, 
		const std::string& postage)
	{
		static const std::string _APITY = "valget";

		std::string guid;
		if(!ParseUtils::ExtractJSONString(js, "guid", guid))
		{
			SendError(con, ErrorTy::Error, "Request to get value missing Equipment GUID.", _APITY, postage);
			return;
		}

		if (!js.contains("gets") || !js["gets"].is_array())
		{
			SendError(con, ErrorTy::Error, "Expected gets member as array with target ids to retrieve.", _APITY, postage);
			return;
		}

		EquipmentListSPtr eqlst = this->coreSys->GetEquipmentCache();
		SEquipmentSPtr sender = eqlst->FindConnection(con);

		if (sender == nullptr)
		{
			// Sanity check. How HandleRTAPI_ValGet would be invoked, since the
			// delegation method that invokes this checks for registration, is
			// uncertain.
			SendError(con, ErrorTy::Error, "Requestor not recognized as Registered", _APITY, postage);
			return;
		}

		VirtualEquipment targ;
		// Make sure the guid matches a valid device. If guid "self" is 
		// a special guid keyword that resolves the the Equipment to the
		// one tied to the connection
		if (guid == "self")
			targ.SetEquipment(sender);
		else if (guid == "system")
			targ.SetCache(this->coreSys->GetDataCache());
		else
			targ.SetEquipment(eqlst->FindGUID(guid));

		if (!targ.IsValid())
		{
			SendError(con, ErrorTy::Error, "Target equipment is not Registered.", _APITY, postage);
			return;
		}

		json ret;
		ret["apity"] = "valget";
		ResponseUtils::ApplyPostage(ret, postage);
		ret["reqguid"] = guid;
		ret["guid"] = targ.GUID();


		bool invalidGets = false;
		std::vector<std::string> unknownNames;
		//
		json jsgets = json::array();
		for (const json j : js["gets"])
		{
			if (!j.is_string())
			{
				invalidGets = true;
				continue;
			}

			std::string paramName = (std::string)j;

			ParamSPtr p = targ.GetParam(paramName);
			if (p == nullptr)
			{
				unknownNames.push_back(paramName);
				continue;
			}
			
			json jsval;
			jsval["id"] = p->GetID();
			jsval["type"] = p->GetJSONType();

			switch (p->Type())
			{
			case DataType::Bool:
				{
					bool bv;
					p->GetValue(bv);
					jsval["val"] = bv;
				}
				break;

			case DataType::String:
			case DataType::Enum:
				{
					std::string sv;
					p->GetValue(sv);
					jsval["val"] = sv;
				}
				break;

			case DataType::Float:
				{
					float fv;
					p->GetValue(fv);
					jsval["val"] = fv;
				}
				break;

			case DataType::Int:
				{
					int iv;
					p->GetValue(iv);
					jsval["val"] = iv;
				}
				break;

			default:
				jsval["val"] = "_unknown";
			}
			jsgets.push_back(jsval);
		}
		ret["gets"] = jsgets;

		// If there are any issues, report those too.
		if (invalidGets || unknownNames.size() > 0)
		{
			json jsissues = json::array();

			if (invalidGets)
				jsissues.push_back("Bad_Gets");

			for (const std::string& un : unknownNames)
				jsissues.push_back("No_Record " + un);

			ret["issues"] = jsissues;
		}
		
		SendJSON(con, ret);
	}

	void BackboneWS::HandleRTAPI_ValSet(
		WSConSPtr con, 
		const json& js, 
		const std::string& postage)
	{
		static const std::string _APITY = "valset";
		LogSys::LogVerbose("Responding to valset request");

		std::string guid;
		if (!ParseUtils::ExtractJSONString(js, "guid", guid))
		{
			SendError(con, ErrorTy::Error, "Request to get value missing Equipment GUID.", _APITY, postage);
			return;
		}

		EquipmentListSPtr eqlst = this->coreSys->GetEquipmentCache();
		SEquipmentSPtr sender = eqlst->FindConnection(con);

		if (!js.contains("sets") || !js["sets"].is_object())
		{
			SendError(con, ErrorTy::Error, "Expected sets member as object with target ids to retrieve.", _APITY, postage);
			return;
		}

		if (sender == nullptr)
		{
			// Sanity check. How HandleRTAPI_ValGet would be invoked, since the
			// delegation method that invokes this checks for registration, is
			// uncertain.
			SendError(con, ErrorTy::Error, "Requestor not recognized as Registered", _APITY, postage);
			return;
		}

		VirtualEquipment targ;
		// Make sure the guid matches a valid device. If guid "self" is 
		// a special guid keyword that resolves the the Equipment to the
		// one tied to the connection
		if (guid == "self")
			targ.SetEquipment(sender);
		else if (guid == "system")
			targ.SetCache(this->coreSys->GetDataCache());
		else
			targ.SetEquipment(eqlst->FindGUID(guid));

		if (!targ.IsValid())
		{
			SendError(con, ErrorTy::Error, "Target equipment is not Registered.", _APITY, postage);
			return;
		}

		// A collection of each attempt and its return code
		// <value, success>
		std::map<std::string, std::string> successes;

		// A collection of successfully changed values, whos changes
		// will be broadcasted.
		std::set<std::string> changedids;
		std::set<std::string> submitids;

		// Set the values
		//////////////////////////////////////////////////
		json jsets = js["sets"];
		for (json::const_iterator it = jsets.begin(); it != jsets.end(); ++it)
		{
			std::string id = it.key();
			ParamSPtr p = targ.GetParam(id);
			if (p == nullptr)
			{
				successes[id] = "notexist";
				continue;
			}

			SetRet setret = SetRet::Invalid;
			if (it->is_boolean())
				setret = p->SetValue((bool)*it);
			else if (it->is_string())
				setret = p->SetValue((std::string)*it);
			else if (it->is_number_integer())
				setret = p->SetValue((int)*it);
			else if (it->is_number_float())
				setret = p->SetValue((float)*it);
			else
			{
				successes[id] = "unknownvalue";
				continue;
			}

			if(setret == SetRet::Success)
			{
				successes[id] = "success";
				changedids.insert(id);
			}
			else if(setret == SetRet::Submit)
			{
				successes[id] = "submit";
				submitids.insert(id);
			}
			else if(setret == SetRet::Invalid)
			{ 
				successes[id] = "fail";
			}
			
			
		}

		// Send response to requestor
		//////////////////////////////////////////////////
		json jsreqp;
		jsreqp["apity"] = "valset";
		jsreqp["guid"] = guid;

		json jssets = json::object();	// A dictionary of the change status per-variable
		json jsids = json::array();		// A copy of the requested IDs are sent back

		ResponseUtils::ApplyPostage(jsreqp, postage);
		for (json::const_iterator it = jsets.begin(); it != jsets.end(); ++it)
		{
			std::string id = it.key();
			jsids.push_back(id);

			json jsvar;
			jsvar["status"] = successes[id];

			ParamSPtr p = targ.GetParam(id);
			if (p != nullptr)
				jsvar["val"] = p->GetValueJSON();

			jssets[id] = jsvar;
		}

		jsreqp["ids"] = jsids;
		jsreqp["sets"] = jssets;
		SendJSON(con, jsreqp);

		// Broadcast update to everyone else.
		//
		// Don't show everything that was requested to change,
		// only things that made a difference.
		//////////////////////////////////////////////////
		if (changedids.size() > 0)
		{
			json broadupdt; // Broadcasted changes
			broadupdt["apity"] = "changedval";
			broadupdt["guid"] = targ.GUID();
			broadupdt["invoker"] = sender->GUID();
			broadupdt["ids"] = jsids;
			json jsbrvals = json::object(); // Broadcasted values
			for (const std::string& chid : changedids)
			{
				ParamSPtr p = targ.GetParam(chid);
				if (p == nullptr)
					continue;

				json jsval;
				jsval["id"] = chid;
				jsval["status"] = "success";
				jsval["val"] = p->GetValueJSON();

				jsbrvals[chid] = jsval;
			}
			for(const std::string& chid: submitids)
			{
				ParamSPtr p = targ.GetParam(chid);
				if (p == nullptr)
					continue;

				json jsval;
				jsval["id"] = chid;
				jsval["status"] = "submit";
				jsval["val"] = p->GetValueJSON();

				jsbrvals[chid] = jsval;
			}
			broadupdt["sets"] = jsbrvals;

			// It's arguable if we should filter out the original
			// sender or not. On the one hand they already get a 
			// response with that data. On the other hand, that
			// means EVERY client needs to explicitly process it
			// instead of a single changeval handler for everything.
			eqlst->Broadcast(broadupdt.dump(), con);
		}

	}

	void BackboneWS::HandleRTAPI_Arm(
		WSConSPtr con, 
		const json& js, 
		const std::string& postage)
	{
	}

	void BackboneWS::HandleRTAPI_Disarm(
		WSConSPtr con, 
		const json& js, 
		const std::string& postage)
	{
	}

	void BackboneWS::HandleRTAPI_Purpose(
		WSConSPtr con, 
		const json& js, 
		const std::string& postage)
	{
		static const std::string _APITY = "purpose";

		if (!js.contains("equipments") || !js["equipments"].is_array())
		{
			SendError(con, ErrorTy::Error, "Purpose listing is missing expected equipment array.", _APITY, postage);
			return;
		}

		
		std::set<std::string> found;
		std::set<std::string> missing;
		std::set<std::string> all;
		std::map<std::string, std::vector<SEquipmentSPtr>> sorted;

		for (json itjs : js["equipments"])
		{
			if (!itjs.is_string())
				continue;

			std::string eqid = (std::string)itjs;
			all.insert(eqid);
			// Missing starts completely full, elements
			// will be removed as they are found.
			missing.insert(eqid);
		}

		json resp;
		resp["apity"] = _APITY;
		ResponseUtils::ApplyPostage(resp, postage);

		// Find equipment with matching purpose.
		EquipmentListSPtr eqs = this->coreSys->GetEquipmentCache();
		// First pass finds matches and sorts them
		for (auto iteq : eqs->itemsByGUID)
		{
			// Is the equipments purpose contained in the set
			// of stuff we're looking out for?
			if (all.find(iteq.second->Purpose()) != all.end())
			{
				SEquipmentSPtr eq = iteq.second;
				std::string purpose = eq->Purpose();
				found.insert(purpose);
				missing.erase(purpose);

				sorted[purpose].push_back(eq);
			}
		}
		// Second pass writes out the JSON
		json reteqs;
		for (auto itsort : sorted)
		{
			json retsort = json::array();
			for (SEquipmentSPtr eq : itsort.second)
				retsort.push_back(eq->GetJSON());

			reteqs[itsort.first] = retsort;
		}
		resp["equipment"] = reteqs;

		// Provide summaries of what was found.
		json jsfound = json::array();
		for (const std::string& str : found)
			jsfound.push_back(str);
		json jsmissing = json::array();
		for (const std::string& str : missing)
			jsmissing.push_back(str);
		//
		resp["found"] = jsfound;
		resp["missing"] = jsmissing;

		SendJSON(con, resp);
	}

	void BackboneWS::HandleRTAPI_Publish(WSConSPtr con, const json& js, const std::string& postage)
	{
		static const std::string _APITY = "publish";

		if (!js.contains("data"))
		{
			this->SendError(con, ErrorTy::Error, "publish's data member is missing.", postage, _APITY);
			return;
		}
		if (!js["data"].is_object())
		{
			this->SendError(con, ErrorTy::Error, "publish's data member isn't an object.", postage, _APITY);
			return;
		}

		std::vector<std::string> recipients;

		json jsMsg;
		jsMsg["apity"] = "msg";
		jsMsg["data"] = js["data"];

		EquipmentListSPtr equips = this->coreSys->GetEquipmentCache();

		if (js.contains("guids"))
		{
			if (!js["guids"].is_array())
			{
				this->SendError(con, ErrorTy::Error, "publish's guid member isn't an array.", postage, _APITY);
				return;
			}

			for (const json& jsguid : js["guids"])
			{
				if (jsguid.is_string())
					recipients.push_back((std::string)jsguid);
			}

			jsMsg["mode"] = "guids";
			
		}
		else if (js.contains("topic"))
		{
			if(!js["topic"].is_string())
			{
				this->SendError(con, ErrorTy::Error, "publish's topic member must be a string.", postage, _APITY);
				return;
			}

			std::string topic = js["topic"];

			for (auto it : equips->itemsByGUID)
			{
				if (it.second->IsSubscribed(topic))
					recipients.push_back(it.first);
			}

			jsMsg["mode"] = "topic";
			jsMsg["topic"] = topic;
		}
		else
		{
			this->SendError(con, ErrorTy::Error, "publish request must either have a guids or topic member..", postage, _APITY);
			return;
		}

		bool anyrecepts = false;
		if (recipients.size() >= 0)
		{
			std::string respStr = jsMsg.dump();
			for (std::string guidsrcvr : recipients)
			{
				SEquipmentSPtr eqrcvr = equips->FindGUID(guidsrcvr);
				if (eqrcvr == nullptr)
					continue;

				this->SendString(eqrcvr->GetSocket(), respStr);
				anyrecepts = true;
			}
		}

		// Send a response to the sender to confirm the request 
		// was handled, as well as letting them know if anyone 
		// received it.
		//
		// For now we don't give much more information back. If
		// they want to track more information, the requestor can
		// track via postage.
		json jsresp;
		jsresp["apity"] = _APITY;
		ResponseUtils::ApplyPostage(jsMsg, postage);
		jsresp["anyrecv"] = anyrecepts ? true : false;
		this->SendJSON(con, jsresp);
	}

	void BackboneWS::HandleRTAPI_Sub(WSConSPtr con, const json& js, const std::string& postage)
	{
		static const std::string _APITY = "subscribe";

		if(!js.contains("mode") || !js["mode"].is_string())
		{
			this->SendError(con, ErrorTy::Error, "subscribe must have a mode member of either add, rem, or report.", postage, _APITY);
			return;
		}

		std::string mode = js["mode"];
		if (mode != "add" && mode != "rem" && mode != "report")
		{
			this->SendError(con, ErrorTy::Error, "subscribe must have a mode member of either add, rem, or report.", postage, _APITY);
			return;
		}

		// The Equipment client may want to check it us to see which
		// topics they are subscribed to
		if (mode == "report")
		{
			EquipmentListSPtr eqLst = this->coreSys->GetEquipmentCache();
			SEquipmentSPtr eqReport = eqLst->FindConnection(con);

			// Sanity check. Shouldn't ever actually happen.
			if (eqReport == nullptr)
			{
				SendError(con, ErrorTy::Error, "Could not verify registration.", _APITY, postage);
				return;
			}

			json jsResp;
			jsResp["apity"] = _APITY;
			jsResp["mode"] = "report";
			// It's uncertain if we should do this just for consistency with rem and add
			jsResp["status"] = "success"; 
			ResponseUtils::ApplyPostage(jsResp, postage);
			json jstopics = json::array();
			for (const std::string& s : eqReport->Subscriptions())
				jstopics.push_back(s);
			jsResp["topics"] = jstopics;
			this->SendJSON(con, jsResp);
			return;
		}

		// Both add and rem need a "topics" array
		if (!js.contains("topics") || !js["topics"].is_array())
		{
			this->SendError(con, ErrorTy::Error, "subscribe must have a mode topics member of type array.", postage, _APITY);
			return;
		}

		// We're collecting these as a set, which will be converted
		// to a vector later. It might seem more practical to just
		// use a vector, but we're leveraging the unique member 
		// property to guard against duplicates.
		std::set<std::string> topics;
		for (const json& jst : js["topics"])
		{
			if (!jst.is_string())
				continue;

			topics.insert((std::string)jst);
		}

		bool any = false;

		EquipmentListSPtr eqs = this->coreSys->GetEquipmentCache();
		SEquipmentSPtr eqReq = eqs->FindConnection(con);

		if (eqReq == nullptr)
		{ 
			// Sanity check,
			// Not expected to ever happen.
			this->SendError(con, ErrorTy::Error, "Corrupted state detected while modifying subscriptions.", _APITY, postage);
			return;
		}

		// Record what changes were successful.
		bool success = false;
		if (mode == "add")
		{
			std::vector<std::string> v(topics.begin(), topics.end());
			success = eqReq->Subscribe(v);
		}
		if (mode == "rem")
		{
			std::vector<std::string> v(topics.begin(), topics.end());
			success = eqReq->Unsubscribe(v);
		}

		json jsresp;
		jsresp["apity"] = _APITY;
		ResponseUtils::ApplyPostage(jsresp, postage);
		jsresp["mode"] = mode;
		jsresp["status"] = success ? "success" : "fail";
		this->SendJSON(con, jsresp);
	}

	void BackboneWS::HandleRTAPI_Datacache(WSConSPtr con, const json& js, const std::string& postage)
	{
		// The apity datacache handler. Only adding variables is supported
		// because modifying and querying variables is done by reusing the
		// varset and varget with a "system" guid.

		static const std::string _APITY = "datacache";

		if (!js.contains("mode") || !js["mode"].is_string())
		{
			this->SendError(con, ErrorTy::Error, "datacache must have a mode member of add.", postage, _APITY);
			return;
		}

		EquipmentListSPtr lst = this->coreSys->GetEquipmentCache();
		SEquipmentSPtr eqInkvoker = lst.get()->FindConnection(con);
		if (eqInkvoker == nullptr)
		{
			// Sanity check, shouldn't ever happen.
			this->SendError(con, ErrorTy::Error, "Could not validate registration.", postage, _APITY);
			return;
		}

		std::string mode = js["mode"];
		// A request to add datacache values
		if (mode == "add")
		{
			if(!js.contains("params") || !js["params"].is_array())
			{
				this->SendError(con, ErrorTy::Error, "datacache adding must have a params member of type array, containing Param definitions.", postage, _APITY);
				return;
			}

			std::vector<std::string> errors;
			std::vector<std::string> success;
			this->coreSys->RegisterDataCacheArray(js["params"], errors, success);
			
			json jsresp;
			jsresp["apity"] = _APITY;
			ResponseUtils::ApplyPostage(jsresp, postage);
			jsresp["mode"] = mode;

			// Three types of success statuses to chooce from, depending
			// on if everything was success, partially successful, or if
			// only failures.
			if (errors.size() > 0 && success.size() == 0)
				jsresp["status"] = "fail";
			else if (success.size() > 0 && errors.size() == 0)
				jsresp["status"] = "success";
			else
				jsresp["status"] = "mixed";

			// Report successfully added Param ids
			json jsAdded = json::array();
			for (const std::string& s : success)
				jsAdded.push_back(s);
			jsresp["ids"] = jsAdded;

			// And errors
			json jsErrs = json::array();
			for (const std::string& s : errors)
				jsErrs.push_back(s);
			jsresp["errors"] = jsErrs;

			this->SendJSON(con, jsresp);

			// Double check 1 more time that there were any successes,
			// but validated by actually querying into the cache.
			bool any = false;
			// And broadcast to everybody else the added datacaches
			if (success.size() > 0)
			{
				ParamCacheSPtr dataCache = this->coreSys->GetDataCache();
				json jsNotif;
				jsNotif["apity"] = "addedcache";
				jsNotif["guid"] = eqInkvoker->GUID();

				json jsids = json::array();
				json jsparams;
				for (const std::string& s : success)
				{
					ParamSPtr p = dataCache->Get(s);
					if (p == nullptr)
						continue;

					jsids.push_back(s);
					jsparams[s] = p->GetJSONDef();
					any = true;
				}
				jsNotif["ids"] = jsids;
				jsNotif["params"] = jsparams;

				if (any)
				{
					// Update to be extra sure we're sending it to everyone relevant.
					// Before we needed it just to get the invoker's guid.
					lst = this->coreSys->GetEquipmentCache();
					std::string strnotif = jsNotif.dump();
					lst->Broadcast(strnotif, con);
				}
			}
		}
		else
		{
			this->SendError(con, ErrorTy::Error, "datacache must have a mode member of add", postage, _APITY);
			return;
		}
	}

	void BackboneWS::Ping()
	{
		json jsping;
		jsping["apity"] = "ping";
		std::string strping = jsping.dump();

		{
			const std::lock_guard<std::mutex> lock(this->unregisteredMutex);
			for (auto it : this->unregistered)
				SendString(it, strping);
		}

		EquipmentListSPtr equips = this->coreSys->GetEquipmentCache();
		equips->Broadcast(strping);
	}
}