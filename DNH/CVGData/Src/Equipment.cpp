#include "Equipment.h"

namespace CVG 
{
	Equipment::Equipment(
		const std::string& name,
		const std::string& manufacturer,
		const std::string& purpose,
		const std::string& hostname,
		EQType type,
		std::vector<ParamSPtr> params,
		json clientData)
	{
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
}