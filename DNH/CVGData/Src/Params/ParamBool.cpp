#include "ParamBool.h"

namespace CVG {

	ParamBool::ParamBool(
		const std::string& id,
		const std::string& label,
		const std::string& category,
		const std::string& unit,
		bool value,
		boost::optional<bool> defVal,
		boost::optional<bool> failVal)
		: Param(id, label, category, unit, DataType::Bool)
	{
		this->curVal = value;
		this->defVal = defVal;
		this->failVal = failVal;
	}

	void ParamBool::ModifyJSONImpl(json& js)
	{
		js["current"] = this->curVal;

		if(this->defVal)
			js["default"] = this->defVal.get();

		if (this->failVal)
			js["fail"] = this->failVal.get();
	}

	std::string ParamBool::StrValue() 
	{
		return this->curVal ? "True" : "False";
	}

	bool ParamBool::HasMin() 
	{
		return false;
	}

	bool ParamBool::HasMax() 
	{
		return false;
	}

	bool ParamBool::HasFail() 
	{
		return this->failVal != boost::none;
	}

	SetRet ParamBool::SetValue(int value, ValTy ty) 
	{
		switch (ty)
		{
		case ValTy::Current:
			this->curVal = IntToBool(value);
			return SetRet::Success;

		case ValTy::Default:
			this->defVal = IntToBool(value);
			return SetRet::Success;

		case ValTy::Fail:
			this->failVal = IntToBool(value);
			return SetRet::Success;
		}
		return SetRet::Invalid;
	}

	bool ParamBool::GetValue(int& value, ValTy ty) 
	{
		switch (ty)
		{
		case ValTy::Current:
			value = BoolToInt(this->curVal);
			return true;

		case ValTy::Default:
			if (this->defVal == boost::none)
				return false;

			value = BoolToInt(this->defVal.get());
			return true;

		case ValTy::Fail:
			if (this->failVal == boost::none)
				return false;

			value = BoolToInt(this->failVal.get());
			return true;
		}
		return false;
	}

	SetRet ParamBool::SetValue(float value, ValTy ty) 
	{
		switch (ty)
		{
		case ValTy::Current:
			this->curVal = FloatToBool(value);
			return SetRet::Success;

		case ValTy::Default:
			this->defVal = FloatToBool(value);
			return SetRet::Success;

		case ValTy::Fail:
			this->failVal = FloatToBool(value);
			return SetRet::Success;
		}
		return SetRet::Invalid;
	}

	bool ParamBool::GetValue(float& value, ValTy ty) 
	{
		switch (ty)
		{
		case ValTy::Current:
			value = BoolToFloat(this->curVal);
			return true;

		case ValTy::Default:
			if (this->defVal == boost::none)
				return false;

			value = BoolToFloat(this->defVal.get());
			return true;

		case ValTy::Fail:
			if (this->failVal == boost::none)
				return false;

			value = BoolToFloat(this->failVal.get());
			return true;
		}
		return false;
	}

	SetRet ParamBool::SetValue(const std::string & value, ValTy ty) 
	{
		switch (ty)
		{
		case ValTy::Current:
			this->curVal = StringToBool(value);
			return SetRet::Success;

		case ValTy::Default:
			this->defVal = StringToBool(value);
			return SetRet::Success;

		case ValTy::Fail:
			this->failVal = StringToBool(value);
			return SetRet::Success;
		}
		return SetRet::Invalid;
	}

	bool ParamBool::GetValue(std::string& value, ValTy ty) 
	{
		switch (ty)
		{
		case ValTy::Current:
			value = BoolToString(this->curVal);
			return true;

		case ValTy::Default:
			if (this->defVal == boost::none)
				return false;

			value = BoolToString(this->defVal.get());
			return true;

		case ValTy::Fail:
			if (this->failVal == boost::none)
				return false;

			value = BoolToString(this->failVal.get());
			return true;
		}
		return false;
	}

	SetRet ParamBool::SetValue(bool value, ValTy ty) 
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
		}
		return SetRet::Invalid;
	}

	bool ParamBool::GetValue(bool& value, ValTy ty) 
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

	SetRet ParamBool::ResetToDefault()
	{
		if (this->defVal == boost::none)
			return SetRet::Invalid;

		this->curVal = this->defVal.get();
		return SetRet::Success;
	}

	ParamSPtr ParamBool::Clone() const
	{
		ParamBool* pb = new ParamBool(*this);
		return ParamSPtr(pb);
	}
}