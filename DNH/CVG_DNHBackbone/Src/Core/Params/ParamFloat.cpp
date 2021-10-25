#include "ParamFloat.h"

namespace CVG 
{
	ParamFloat::ParamFloat(
		const std::string& id,
		const std::string& label,
		const std::string& category,
		const std::string& unit,
		float value,
		boost::optional<float> defVal,
		boost::optional<float> failVal,
		boost::optional<float> minVal,
		boost::optional<float> maxVal)
		: Param(id, label, category, unit, DataType::Float)
	{
		this->curVal = value;
		this->defVal = defVal;
		this->failVal = failVal;
		this->minVal = minVal;
		this->maxVal = maxVal;
	}

	void ParamFloat::ModifyJSONImpl(json& js)
	{
		js["current"] = this->curVal;

		if(this->defVal.has_value())
			js["default"] = this->defVal.get();

		if (this->failVal.has_value())
			js["fail"] = this->failVal.get();

		if (this->minVal.has_value())
			js["min"] = this->minVal.get();

		if (this->maxVal.has_value())
			js["max"] = this->maxVal.get();
	}


	std::string ParamFloat::StrValue()
	{
		return std::to_string(this->curVal);
	}

	bool ParamFloat::HasMin() 
	{
		return this->minVal.has_value();
	}

	bool ParamFloat::HasMax() 
	{
		return this->maxVal.has_value();
	}

	bool ParamFloat::HasFail() 
	{
		return this->minVal.has_value();
	}

	bool ParamFloat::SetValue(int value, ValTy ty) 
	{
		switch (ty)
		{
		case ValTy::Current:
			this->curVal = value;
			return true;

		case ValTy::Default:
			this->defVal = value;
			return true;

		case ValTy::Fail:
			this->failVal = value;
			return true;

		case ValTy::Max:
			this->maxVal = value;
			return true;

		case ValTy::Min:
			this->minVal = value;
			return true;
		}
		return false;
	}

	bool ParamFloat::GetValue(int& value, ValTy ty) 
	{
		switch (ty)
		{
		case ValTy::Current:
			value = (int)this->curVal;
			return true;

		case ValTy::Default:
			if (!this->defVal.has_value())
				return false;

			value = (int)this->defVal.get();
			return true;

		case ValTy::Fail:
			if (!this->failVal.has_value())
				return false;

			value = (int)this->failVal.get();
			return true;

		case ValTy::Max:
			if (!this->maxVal.has_value())
				return false;

			value = (int)this->maxVal.get();
			return true;

		case ValTy::Min:
			if (!this->minVal.has_value())
				return false;

			value = (int)this->minVal.get();
			return true;
		}
		return false;
	}

	bool ParamFloat::SetValue(float value, ValTy ty) 
	{
		switch (ty)
		{
		case ValTy::Current:
			this->curVal = value;
			return true;

		case ValTy::Default:
			this->defVal = value;
			return true;

		case ValTy::Fail:
			this->failVal = value;
			return true;

		case ValTy::Max:
			this->maxVal = value;
			return true;

		case ValTy::Min:
			this->minVal = value;
			return true;
		}
		return false;
	}

	bool ParamFloat::GetValue(float& value, ValTy ty) 
	{
		switch (ty)
		{
		case ValTy::Current:
			value = this->curVal;
			return true;

		case ValTy::Default:
			if (!this->defVal.has_value())
				return false;

			value = this->defVal.get();
			return true;

		case ValTy::Fail:
			if (!this->failVal.has_value())
				return false;

			value = this->failVal.get();
			return true;

		case ValTy::Max:
			if (!this->maxVal.has_value())
				return false;

			value = this->maxVal.get();
			return true;

		case ValTy::Min:
			if (!this->minVal.has_value())
				return false;

			value = this->minVal.get();
			return true;
		}
		return false;
	}

	bool ParamFloat::SetValue(const std::string& value, ValTy ty) 
	{
		try
		{
			float f = std::stof(value);

			switch (ty)
			{
			case ValTy::Current:
				this->curVal = f;
				return true;

			case ValTy::Default:
				this->defVal = f;
				return true;

			case ValTy::Fail:
				this->failVal = f;
				return true;

			case ValTy::Max:
				this->maxVal = f;
				return true;

			case ValTy::Min:
				this->minVal = f;
				return true;
			}
		}
		catch (std::exception&)
		{
			return false;
		}

		return false;
	}

	bool ParamFloat::GetValue(std::string& value, ValTy ty) 
	{
		switch (ty)
		{
		case ValTy::Current:
			value = this->curVal;
			return true;

		case ValTy::Default:
			if (!this->defVal.has_value())
				return false;

			value = this->defVal.get();
			return true;

		case ValTy::Fail:
			if (!this->failVal.has_value())
				return false;

			value = this->failVal.get();
			return true;

		case ValTy::Max:
			if (!this->maxVal.has_value())
				return false;

			value = this->maxVal.get();
			return true;

		case ValTy::Min:
			if (!this->minVal.has_value())
				return false;

			value = this->minVal.get();
			return true;
		}
		return false;
	}

	bool ParamFloat::SetValue(bool value, ValTy ty) 
	{
		float fb = BoolToFloat(value);

		switch (ty)
		{
		case ValTy::Current:
			this->curVal = fb;
			return true;

		case ValTy::Default:
			this->defVal = fb;
			return true;

		case ValTy::Fail:
			this->failVal = fb;

		case ValTy::Max:
			this->maxVal = fb;

		case ValTy::Min:
			this->minVal = fb;
		}
		return false;
	}

	bool ParamFloat::GetValue(bool& value, ValTy ty) 
	{
		switch (ty)
		{
		case ValTy::Current:
			return true;

		case ValTy::Default:
			return true;

		case ValTy::Fail:
			if (!this->failVal.has_value())
				return false;

			value = FloatToBool(this->failVal.get());
			return true;

		case ValTy::Max:
			if (!this->maxVal.has_value())
				return false;

			value = FloatToBool(this->maxVal.get());
			return true;

		case ValTy::Min:
			if (!this->minVal.has_value())
				return false;

			value = FloatToBool(this->minVal.get());
			return true;
		}
		return false;
	}

	bool ParamFloat::ResetToDefault()
	{
		if (!this->defVal.has_value())
			return false;

		this->curVal = this->defVal.get();
		return true;
	}
}