#include "ParamEnum.h"

namespace CVG {

	ParamEnum::ParamEnum(
		const std::string& id,
		const std::string& label,
		const std::string& category,
		const std::string& unit,
		const std::string& value,
		boost::optional<std::string> defVal,
		boost::optional<std::string> failVal,
		std::vector<std::string> allowedValues)
		: ParamString(id, label, category, unit, value, defVal, failVal, DataType::Enum)
	{
		this->allowedValues = allowedValues;

		if (this->allowedValues.size() == 0)
			this->allowedValues.push_back("!Error_Empty");

		// Even as early as creation, we're enforcing that an enum's various
		// values must be the set of possible values.
		if (!this->ContainsAllowed(this->curVal))
		{
			this->curVal = this->allowedValues[0];
		}
		if (!this->defVal.has_value())
		{
			if (!this->ContainsAllowed(this->defVal.get()))
				this->defVal = this->allowedValues[0];
		}
		if (this->failVal.has_value())
		{
			if (!this->ContainsAllowed(this->failVal.get()))
				this->failVal = this->allowedValues[0];
		}
	}

	bool ParamEnum::ContainsAllowed(const std::string& qval)
	{
		auto itf = 
			std::find(
				this->allowedValues.begin(),
				this->allowedValues.end(),
				qval);

		return itf != this->allowedValues.end();
	}

	void ParamEnum::ModifyJSONImpl(json& js)
	{
		ParamString::ModifyJSONImpl(js);

		json jsPos = json::array();
		for (const std::string& p : this->allowedValues)
			jsPos.push_back(p);

		js["possible"] = jsPos;
	}

	bool ParamEnum::SetValue(int value, ValTy ty)
	{
		std::string si = std::to_string(value);
		if (!this->ContainsAllowed(si))
			return false;

		return this->SetValue(si, ty);
	}

	bool ParamEnum::SetValue(float value, ValTy ty)
	{
		// This makes EXTREEMLY little sense, given how volatile
		// and finicky string output for floats can be. But the
		// implementation doesn't question that, it just does its
		// purpose - why this would ever be used is an issue 
		// whatever invoked this implementation needs to consider.
		std::string sf = std::to_string(value);
		return this->SetValue(sf, ty);
	}

	bool ParamEnum::SetValue(const std::string& value, ValTy ty)
	{
		// Make sure it's valid, before we delegate to the base
		// implementation.
		if (!this->ContainsAllowed(value))
			return false;

		return ParamString::SetValue(value, ty);
	}

	bool ParamEnum::SetValue(bool value, ValTy ty)
	{
		std::string sb = BoolToString(value);
		return SetValue(value, ty);
	}
}