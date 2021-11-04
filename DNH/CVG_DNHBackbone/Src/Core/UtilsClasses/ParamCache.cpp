#include <set>

#include "ParamCache.h"
#include "../Params/ParamUtils.h"

#include "../Params/ParamInt.h"
#include "../Params/ParamFloat.h"
#include "../Params/ParamString.h"
#include "../Params/ParamBool.h"

namespace CVG
{
	ParamSPtr ParamCache::ParseParamJSON(const json& jsparam, std::string& error)
	{
		ParamSPtr ret = ParamUtils::Parse(jsparam, error);
		if (ret == nullptr)
			return nullptr;

		// Toss the param, send back a failure. The id is already taken.
		if (this->Contains(ret->GetID()))
		{
			error = "Param ID " + ret->GetID() + " already taken.";
			return nullptr;
		}

		// Register and return.
		this->params[ret->GetID()] = ret;
		return ret;
	}

	void ParamCache::Clear()
	{
		this->params.clear();
	}

	void ParamCache::Reset(
		bool removeNoDefs, 
		std::set<std::string>* outModified,
		std::set<std::string>* outRemoved)
	{
		std::set<std::string> idsWithNoDefs;
		for (auto it : this->params)
		{
			if (!it.second->ResetToDefault())
				idsWithNoDefs.insert(it.first);
			else if (outModified != nullptr)
				outModified->insert(it.first);
		}

		if (removeNoDefs == true)
		{
			for (const std::string& idRm : idsWithNoDefs)
			{
				this->params.erase(this->params.find(idRm));
				if (outRemoved != nullptr)
					outRemoved->insert(idRm);
			}
		}
	}

	bool ParamCache::Contains(const std::string& id)
	{
		auto itFind = this->params.find(id);
		return itFind != this->params.end();
	}

	ParamSPtr ParamCache::Get(const std::string& id)
	{
		auto itFind = this->params.find(id);
		if (itFind == this->params.end())
			return nullptr;

		return itFind->second;
	}

	bool ParamCache::Set(std::string& paramid, const json& jsVal, bool createifmissing)
	{
		// Go through the gauntlet of possible JSON types that match with
		// a Param::Set(*) overload. Note that even if the type doesn't match,
		// the various Set()s also have their own conversion rules.
		//
		// Note that we need to force the jsVal to be casted in order
		// for it to go through the intended Set() overload.

		if (jsVal.is_string())
			return this->Set(paramid, (std::string)jsVal, createifmissing);

		if (jsVal.is_boolean())
			return this->Set(paramid, (bool)jsVal, createifmissing);

		if (jsVal.is_number_integer())
			return this->Set(paramid, (int)jsVal, createifmissing);

		if (jsVal.is_number_float())
			return this->Set(paramid, (float)jsVal, createifmissing);

		return false;
	}

	bool ParamCache::Set(std::string& paramid, int iVal, bool createifmissing)
	{
		ParamSPtr param = this->Get(paramid);
		if (param == nullptr)
		{
			if (!createifmissing || this->Contains(paramid))
				return false;

			ParamInt* pi = 
				new ParamInt(
					paramid, 
					paramid, 
					std::string(), 
					std::string(),
					iVal, 
					boost::none,
					boost::none,
					boost::none,
					boost::none);

			this->params[paramid] = ParamSPtr(pi);
			return true;
		}

		return param->SetValue(iVal);
	}

	bool ParamCache::Set(std::string& paramid, float fVal, bool createifmissing)
	{
		ParamSPtr param = this->Get(paramid);
		if (param == nullptr)
		{
			if (!createifmissing || this->Contains(paramid))
				return false;

			ParamFloat* pf =
				new ParamFloat(
					paramid,
					paramid,
					std::string(),
					std::string(),
					fVal,
					boost::none,
					boost::none,
					boost::none,
					boost::none);

			this->params[paramid] = ParamSPtr(pf);
			return true;
		}

		return param->SetValue(fVal);
	}

	// String is used for both enum and strings.
	bool ParamCache::Set(std::string& paramid, const std::string& sVal, bool createifmissing)
	{
		ParamSPtr param = this->Get(paramid);
		if (param == nullptr)
		{
			if (!createifmissing || this->Contains(paramid))
				return false;

			ParamString* ps =
				new ParamString(
					paramid,
					paramid,
					std::string(),
					std::string(),
					sVal,
					boost::none,
					boost::none);

			this->params[paramid] = ParamSPtr(ps);
			return true;
		}

		return param->SetValue(sVal);
	}

	bool ParamCache::Set(std::string& paramid, bool bVal, bool createifmissing)
	{
		ParamSPtr param = this->Get(paramid);
		if (param == nullptr)
		{
			if (!createifmissing || this->Contains(paramid))
				return false;

			ParamBool* pb =
				new ParamBool(
					paramid,
					paramid,
					std::string(),
					std::string(),
					bVal,
					boost::none,
					boost::none);

			this->params[paramid] = ParamSPtr(pb);
			return true;
		}

		return param->SetValue(bVal);
	}

	json ParamCache::JSONValueObject() const
	{
		json ret;
		for (auto it : this->params)
			ret[it.first] = it.second->GetValueJSON();
		return ret;
	}

	json ParamCache::JSONDefinitionsArray() const
	{
		json ret = json::array();
		for (auto it : this->params)
			ret.push_back(it.second->GetJSONDef());

		return ret;
	}

	ParamCacheSPtr ParamCache::Clone()
	{
		ParamCacheSPtr ret = std::make_shared< ParamCache>();
		ret->params.insert(this->params.begin(), this->params.end());
		return ret;
	}

	ParamCacheSPtr ParamCache::DeepClone()
	{
		ParamCacheSPtr ret = std::make_shared< ParamCache>();
		for (auto it : this->params)
			ret->params[it.first] = it.second->Clone();

		return ret;
	}
}