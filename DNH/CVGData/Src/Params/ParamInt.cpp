#include "ParamInt.h"

namespace CVG
{
	ParamInt::ParamInt(
		const std::string& id,
		const std::string& label,
		const std::string& category,
		const std::string& unit,
		int value,
		boost::optional<int> defVal,
		boost::optional<int> failVal,
		boost::optional<int> minVal,
		boost::optional<int> maxVal)
		: Param(id, label, category, unit, DataType::Int)
	{
		this->curVal = value;
		this->defVal = value;
		this->failVal = failVal;
		this->minVal = minVal;
		this->maxVal = maxVal;
	}

	void ParamInt::ModifyJSONImpl(json& js)
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

	std::string ParamInt::StrValue()
	{
		return std::to_string(this->curVal);
	}

	bool ParamInt::HasMin()
	{
		return this->minVal != boost::none;
	}

	bool ParamInt::HasMax()
	{
		return this->maxVal != boost::none;
	}

	bool ParamInt::HasFail()
	{
		return this->failVal != boost::none;
	}

	SetRet ParamInt::SetValue(int value, ValTy ty)
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

	bool ParamInt::GetValue(int& value, ValTy ty)
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

	SetRet ParamInt::SetValue(float value, ValTy ty)
	{
		int iv = (int)value;
		switch (ty)
		{
		case ValTy::Current:
			this->curVal = iv;
			return SetRet::Success;

		case ValTy::Default:
			this->defVal = iv;
			return SetRet::Success;

		case ValTy::Fail:
			this->failVal = iv;
			return SetRet::Success;

		case ValTy::Max:
			this->maxVal = iv;
			return SetRet::Success;

		case ValTy::Min:
			this->minVal = iv;
			return SetRet::Success;
		}
		return SetRet::Invalid;
	}

	bool ParamInt::GetValue(float& value, ValTy ty)
	{
		switch (ty)
		{
		case ValTy::Current:
			value = (float)this->curVal;
			return true;

		case ValTy::Default:
			if (this->defVal == boost::none)
				return false;

			value = (float)this->defVal.get();
			return true;

		case ValTy::Fail:
			if (failVal == boost::none)
				return false;

			value = (float)this->failVal.get();
			return true;

		case ValTy::Max:
			if (this->maxVal == boost::none)
				return false;

			value = (float)this->maxVal.get();
			return true;

		case ValTy::Min:
			if (this->minVal == boost::none)
				return false;

			value = (float)this->minVal.get();
			return true;
		}

		return false;
	}

	SetRet ParamInt::SetValue(const std::string & value, ValTy ty)
	{
		try
		{
			int is = std::stoi(value);

			switch (ty)
			{
			case ValTy::Current:
				this->curVal = is;
				return SetRet::Success;

			case ValTy::Default:
				this->defVal = is;
				return SetRet::Success;

			case ValTy::Fail:
				this->failVal = is;
				return SetRet::Success;

			case ValTy::Max:
				this->maxVal = is;
				return SetRet::Success;

			case ValTy::Min:
				this->minVal = is;
				return SetRet::Success;
			}
		}
		catch (const std::exception&) {
			return SetRet::Invalid;
		}

		return SetRet::Invalid;
	}

	bool ParamInt::GetValue(std::string& value, ValTy ty)
	{
		switch (ty)
		{
		case ValTy::Current:
			return true;

		case ValTy::Default:
			return true;

		case ValTy::Fail:
			return true;

		case ValTy::Max:
			break;

		case ValTy::Min:
			break;
		}
		return false;
	}

	SetRet ParamInt::SetValue(bool value, ValTy ty)
	{
		int ib = value ? 1 : 0;

		switch (ty)
		{
		case ValTy::Current:
			this->curVal = ib;
			return SetRet::Success;

		case ValTy::Default:
			this->defVal = ib;
			return SetRet::Success;

		case ValTy::Fail:
			this->failVal = ib;
			return SetRet::Success;

		case ValTy::Max:
			this->maxVal = ib;
			return SetRet::Success;

		case ValTy::Min:
			this->minVal = ib;
			return SetRet::Success;
		}
		return SetRet::Invalid;
	}

	bool ParamInt::GetValue(bool& value, ValTy ty)
	{
		switch (ty)
		{
		case ValTy::Current:
			value = this->curVal != 0;
			return true;

		case ValTy::Default:
			value = this->defVal != 0;
			return true;

		case ValTy::Fail:
			if (this->failVal == boost::none)
				return false;

			value = this->failVal.get() != 0;
			return true;

		case ValTy::Max:
			if (this->maxVal == boost::none)
				return false;

			value = this->maxVal.get() != 0;
			return true;

		case ValTy::Min:
			if (this->minVal == boost::none)
				return false;

			value = this->minVal.get() != 0;
			return true;
		}
		return false;
	}

	SetRet ParamInt::ResetToDefault()
	{
		if (this->defVal == boost::none)
			return SetRet::Invalid;

		this->curVal = this->defVal.get();
		return SetRet::Success;
	}

	ParamSPtr ParamInt::Clone() const
	{
		ParamInt* pi = new ParamInt(*this);
		return ParamSPtr(pi);
	}
}