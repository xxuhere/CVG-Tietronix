#include "Equipment.h"
#include "ParseUtils.h"

namespace CVG 
{
	Equipment::Equipment(
		const std::string& name,
		const std::string& manufacturer,
		const std::string& purpose,
		const std::string& hostname,
		const std::string& guid,
		EQType type,
		std::vector<ParamSPtr> params,
		json clientData)
	{
		this->guid				= guid;
		this->name				= name;
		this->manufacturer		= manufacturer;
		this->purpose			= purpose;
		this->hostname			= hostname;
		this->equipmentType		= type;
		this->paramCache		= params;
		this->clientData		= clientData;
	}

	
	json Equipment::GetJSONSummary() const
	{
		json ret;

		ret["name"] = this->name;
		ret["guid"] = this->guid;

		return ret;
	}

	json Equipment::GetJSON() const
	{
		json ret =
			EquipmentJSONTemplate(
				this->name,
				this->guid,
				this->equipmentType,
				this->purpose);

		ret["manufacturer"] = this->manufacturer;
		
		if(this->hostname.size() > 0)
			ret["hostname"] = this->hostname;

		json jsParams = json::array();
		for (const ParamSPtr pp : this->paramCache)
			jsParams.push_back(pp->GetJSONDef());

		ret["params"] = jsParams;

		for (
			json::const_iterator it = this->clientData.begin();
			it != this->clientData.end();
			++it)
		{
			ret[it.key()] = it.value();
		}	

		return ret;
	}

	Equipment::iterator Equipment::begin()
	{
		return this->paramCache.begin();
	}

	Equipment::iterator Equipment::end()
	{
		return this->paramCache.end();
	}

	ParamSPtr Equipment::GetParam(const std::string& id)
	{
		return this->paramCache.Get(id);
	}

	json Equipment::EquipmentJSONTemplate(
		const std::string& name,
		const std::string& guid,
		EQType ty,
		const std::string& purpose)
	{
		json ret;

		ret["name"] = name;
		ret["guid"] = guid;
		ret["type"] = ConvertToString(ty);
		ret["purpose"] = purpose;

		return ret;
	}

	bool Equipment::ParseEquipmentFields(
		const json& js,
		std::string& guid,
		std::string& manufacturer,
		std::string& name,
		std::string& purpose,
		std::string& type,
		std::string& hostname,
		const json** outParams)
	{
		// Clear everything out, this function has relevant output
		// variables whether-or-not function succeeds.
		guid.clear();
		manufacturer.clear();
		name.clear();
		purpose.clear();
		type.clear();
		*outParams = nullptr;
		bool ret = true;

		// Certain required items will send an error return.
		// Note that doesn't stop the simple data parsing process.
		if(!ParseUtils::ExtractJSONString(js, "type", type))
			ret = false;

		if(!ParseUtils::ExtractJSONString(js, "guid", guid))
			ret = false;

		if (!ParseUtils::ExtractJSONString(js, "name", name))
			ret = false;

		ParseUtils::ExtractJSONString(js, "purpose", purpose);
		ParseUtils::ExtractJSONString(js, "manufacturer", manufacturer);
		ParseUtils::ExtractJSONString(js, "hostname", hostname);

		if(outParams != nullptr && js.contains("params") && js["params"].is_array())
			*outParams = &js["params"];

		return ret;
	}

	bool Equipment::ExtractClientData(json& jsDst, const json& jsSrc)
	{
		// Extra client data is always expected to be in a JSON object.
		if(!jsDst.is_object())
			return false;

		// These are keywords for BOTH the client and server-side. While the
		// Equipment (in-theory) deals with non-server aspects of representing
		// an Equipment, we get more utility out of it if we consider parsing
		// keywords for both server/client. Also, it doesn't create any compile
		// dependencies.
		const static std::set<std::string> alreadyCovered = 
		{
			"apity", 
			"guid", 
			"hostname",
			"type", 
			"manufacturer", 
			"name",
			"params", 
			"purpose", 
			"topics"
		};

		for (json::const_iterator it = jsSrc.begin(); it != jsSrc.end(); ++it)
		{
			if (alreadyCovered.find(it.key()) != alreadyCovered.end())
				continue;

			jsDst[it.key()] = it.value();
		}
		return true;
	}
}