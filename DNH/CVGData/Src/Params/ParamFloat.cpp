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

		if(this->defVal)
			js["default"] = this->defVal.get();

		if (this->failVal)
			js["fail"] = this->failVal.get();

		if (this->minVal)
			js["min"] = this->minVal.get();

		if (this->maxVal)
			js["max"] = this->maxVal.get();
	}


	std::string ParamFloat::StrValue()
	{
		return std::to_string(this->curVal);
	}

	bool ParamFloat::HasMin() 
	{
		return this->minVal != boost::none;
	}

	bool ParamFloat::HasMax() 
	{
		return this->maxVal != boost::none;
	}

	bool ParamFloat::HasFail() 
	{
		return this->minVal != boost::none;
	}

	SetRet ParamFloat::SetValue(int value, ValTy ty) 
	{
		switch (ty)
		{
		case ValTy::Current:
			this->curVal = value;
			return SetRet::Success;

		case ValTy::Default:
			this->defVal = value;
			return SetRet::Success;

		case ValTy::Fail:
			this->failVal = value;
			return SetRet::Success;

		case ValTy::Max:
			this->maxVal = value;
			return SetRet::Success;

		case ValTy::Min:
			this->minVal = value;
			return SetRet::Success;
		}
		return SetRet::Invalid;
	}

	bool ParamFloat::GetValue(int& value, ValTy ty) 
	{
		switch (ty)
		{
		case ValTy::Current:
			value = (int)this->curVal;
			return true;

		case ValTy::Default:
			if (this->defVal == boost::none)
				return false;

			value = (int)this->defVal.get();
			return true;

		case ValTy::Fail:
			if (this->failVal == boost::none)
				return false;

			value = (int)this->failVal.get();
			return true;

		case ValTy::Max:
			if (this->maxVal == boost::none)
				return false;

			value = (int)this->maxVal.get();
			return true;

		case ValTy::Min:
			if (this->minVal == boost::none)
				return false;

			value = (int)this->minVal.get();
			return true;
		}
		return false;
	}

	SetRet ParamFloat::SetValue(float value, ValTy ty) 
	{
		switch (ty)
		{
		case ValTy::Current:
			this->curVal = value;
			return SetRet::Success;

		case ValTy::Default:
			this->defVal = value;
			return SetRet::Success;

		case ValTy::Fail:
			this->failVal = value;
			return SetRet::Success;

		case ValTy::Max:
			this->maxVal = value;
			return SetRet::Success;

		case ValTy::Min:
			this->minVal = value;
			return SetRet::Success;
		}
		return SetRet::Invalid;
	}

	bool ParamFloat::GetValue(float& value, ValTy ty) 
	{
		switch (ty)
		{
		case ValTy::Current:
			value = this->curVal;
			return true;

		case ValTy::Default:
			if (this->defVal == boost::none)
				return false;

			value = this->defVal.get();
			return true;

		case ValTy::Fail:
			if (this->failVal == boost::none)
				return false;

			value = this->failVal.get();
			return true;

		case ValTy::Max:
			if (this->maxVal == boost::none)
				return false;

			value = this->maxVal.get();
			return true;

		case ValTy::Min:
			if (this->minVal == boost::none)
				return false;

			value = this->minVal.get();
			return true;
		}
		return false;
	}

	SetRet ParamFloat::SetValue(const std::string& value, ValTy ty) 
	{
		try
		{
			float f = std::stof(value);

			switch (ty)
			{
			case ValTy::Current:
				this->curVal = f;
				return SetRet::Success;

			case ValTy::Default:
				this->defVal = f;
				return SetRet::Success;

			case ValTy::Fail:
				this->failVal = f;
				return SetRet::Success;

			case ValTy::Max:
				this->maxVal = f;
				return SetRet::Success;

			case ValTy::Min:
				this->minVal = f;
				return SetRet::Success;
			}
		}
		catch (std::exception&)
		{
			return SetRet::Invalid;
		}

		return SetRet::Invalid;
	}

	bool ParamFloat::GetValue(std::string& value, ValTy ty) 
	{
		switch (ty)
		{
		case ValTy::Current:
			value = this->curVal;
			return true;

		case ValTy::Default:
			if (this->defVal == boost::none)
				return false;

			value = this->defVal.get();
			return true;

		case ValTy::Fail:
			if (this->failVal == boost::none)
				return false;

			value = this->failVal.get();
			return true;

		case ValTy::Max:
			if (this->maxVal == boost::none)
				return false;

			value = this->maxVal.get();
			return true;

		case ValTy::Min:
			if (this->minVal == boost::none)
				return false;

			value = this->minVal.get();
			return true;
		}
		return false;
	}

	SetRet ParamFloat::SetValue(bool value, ValTy ty) 
	{
		float fb = BoolToFloat(value);

		switch (ty)
		{
		case ValTy::Current:
			this->curVal = fb;
			return SetRet::Success;

		case ValTy::Default:
			this->defVal = fb;
			return SetRet::Success;

		case ValTy::Fail:
			this->failVal = fb;
			return SetRet::Success;

		case ValTy::Max:
			this->maxVal = fb;
			return SetRet::Success;

		case ValTy::Min:
			this->minVal = fb;
			return SetRet::Success;
		}
		return SetRet::Invalid;
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
			if (this->failVal == boost::none)
				return false;

			value = FloatToBool(this->failVal.get());
			return true;

		case ValTy::Max:
			if (this->maxVal == boost::none)
				return false;

			value = FloatToBool(this->maxVal.get());
			return true;

		case ValTy::Min:
			if (this->minVal == boost::none)
				return false;

			value = FloatToBool(this->minVal.get());
			return true;
		}
		return false;
	}

	SetRet ParamFloat::ResetToDefault()
	{
		if (this->defVal == boost::none)
			return SetRet::Invalid;

		this->curVal = this->defVal.get();
		return SetRet::Success;
	}

	ParamSPtr ParamFloat::Clone() const
	{
		ParamFloat* pf = new ParamFloat(*this);
		return ParamSPtr(pf);
	}
}