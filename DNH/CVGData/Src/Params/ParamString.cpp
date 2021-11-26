#include "ParamString.h"

namespace CVG {

	ParamString::ParamString(
		const std::string& id,
		const std::string& label,
		const std::string& category,
		const std::string& unit,
		const std::string& value,
		boost::optional<std::string> defVal,
		boost::optional<std::string> failVal)
		: Param(id, label, category, unit, DataType::String)
	{
		this->curVal = value;
		this->defVal = value;
		this->failVal = failVal;
	}

	ParamString::ParamString(
		const std::string& id,
		const std::string& label,
		const std::string& category,
		const std::string& unit,
		const std::string& value,
		boost::optional<std::string> defVal,
		boost::optional<std::string> failVal,
		DataType ty)
		: Param(id, label, category, unit, ty)
	{
		this->curVal = value;
		this->defVal = defVal;
		this->failVal = failVal;
	}

	void ParamString::ModifyJSONImpl(json& js)
	{
		js["current"] = this->curVal;

		if(this->defVal)
			js["default"] = this->defVal.get();

		if (this->failVal)
			js["fail"] = this->failVal.get();
	}

	std::string ParamString::StrValue()
	{
		return this->curVal;
	}

	bool ParamString::HasMin()
	{
		return false;
	}

	bool ParamString::HasMax()
	{
		return false;
	}

	bool ParamString::HasFail()
	{
		return this->failVal != boost::none;
	}

	bool ParamString::SetValue(int value, ValTy ty)
	{
		switch (ty)
		{
		case ValTy::Current:
			this->curVal = std::to_string(value);
			return true;

		case ValTy::Default:
			this->defVal = std::to_string(value);
			return true;

		case ValTy::Fail:
			this->failVal = std::to_string(value);
			return true;
		}

		return false;
	}

	bool ParamString::GetValue(int& value, ValTy ty)
	{
		try {
			switch (ty) 
			{
			case ValTy::Current:
				value = std::stoi(this->curVal);
				return true;

			case ValTy::Default:
				if (this->defVal == boost::none)
					return false;

				value = std::stoi(this->defVal.get());
				return true;

			case ValTy::Fail:
				if (this->failVal == boost::none)
					return false;

				value = std::stoi(this->failVal.get());
				return true;
			}
		}
		catch(const std::exception&) {
			return false;
		}

		return false;
	}

	bool ParamString::SetValue(float value, ValTy ty)
	{
		try {
			switch (ty) 
			{
			case ValTy::Current:
				this->curVal = std::to_string(value);
				return true;

			case ValTy::Default:
				this->defVal = std::to_string(value);

			case ValTy::Fail:
				this->failVal = std::to_string(value);
			}
		}
		catch (const std::exception&) {
			// If conversion fails, it's not a system error, 
			// but the request is ignored and that is reported.
			return false;
		}

		return false;
	}

	bool ParamString::GetValue(float& value, ValTy ty)
	{
		try {
			switch (ty)
			{
			case ValTy::Current:
				value = std::stof(this->curVal);
				return true;

			case ValTy::Default:
				if (this->defVal == boost::none)
					return false;

				value = std::stof(this->defVal.get());
				return true;

			case ValTy::Fail:
				if (this->failVal == boost::none)
					return false;
				value = std::stof(this->failVal.get());
				return true;
			}
		}
		catch (const std::exception&) 
		{
			// If conversion fails, it's not a system error, 
			// but the request is ignored and that is reported.
			return false;
		}

		return false;
	}

	bool ParamString::SetValue(const std::string & value, ValTy ty) 
	{
		switch (ty)
		{
		case ValTy::Current:
			this->curVal = value;
			break;

		case ValTy::Default:
			this->defVal = value;
			break;

		case ValTy::Fail:
			this->failVal = value;
		}

		return true;
	}

	bool ParamString::GetValue(std::string& value, ValTy ty) 
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
		}

		return false;
	}

	bool ParamString::SetValue(bool value, ValTy ty) 
	{
		switch (ty)
		{
		case ValTy::Current:
			this->curVal = BoolToString(value);
			return true;

		case ValTy::Default:
			this->curVal = BoolToString(value);
			return true;

		case ValTy::Fail:
			this->failVal = BoolToString(value);
			return true;
		}

		return false;
	}

	bool ParamString::GetValue(bool& value, ValTy ty) 
	{
		switch (ty)
		{
		case ValTy::Current:
			value = StringToBool(this->curVal);
			return true;

		case ValTy::Default:
			if (this->defVal == boost::none)
				return false;

			value = StringToBool(this->defVal.get());
			return true;

		case ValTy::Fail:
			if (this->failVal == boost::none)
				return false;

			value = StringToBool(this->failVal.get());
			return true;
		}

		return false;
	}

	bool ParamString::ResetToDefault()
	{
		if (this->defVal == boost::none)
			return false;

		this->curVal = this->defVal.get();
		return true;
	}

	ParamSPtr ParamString::Clone() const
	{
		ParamString* ps = new ParamString(*this);
		return ParamSPtr(ps);
	}
}