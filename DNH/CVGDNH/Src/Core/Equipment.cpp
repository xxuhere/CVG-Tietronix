#include "Equipment.h"

namespace CVG 
{
	Equipment::iterator::iterator(std::vector<ParamSPtr>::iterator it)
	{
		this->it = it;
	}

	ParamSPtr Equipment::iterator::operator *()
	{
		return *it;
	}

	void Equipment::iterator::operator++()
	{
		this->it++;
	}

	voipud Equipment::iterator::operator--()
	{
		this->it--;
	}

	bool Equipment::iterator::operator == (const iterator& itOther) const
	{
		return this->it == itOther.it;
	}

	bool Equipment::iterator::operator != (const iterator& itOther) const
	{
		return this->it != itOther.it;
	}

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
		this->params			= params;
		this->clientData		= clientData;
	}

	Equipment::iterator Equipment::begin()
	{
		return Equipment::iterator(this->params.begin());
	}

	Equipment::iterator Equipment::end()
	{
		return Equipment::iterator(this->params.end());
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
		for (ParamSPtr pp : this->params)
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

	ParamSPtr Equipment::GetParam(const std::string& id)
	{
		for (ParamSPtr p : this->params)
		{
			if (p->GetID() == id)
				return p;
		}
		return nullptr;
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