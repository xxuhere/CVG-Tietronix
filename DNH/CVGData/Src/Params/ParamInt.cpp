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
		return !!this->minVal;
	}

	bool ParamInt::HasMax()
	{
		return !!this->maxVal;
	}

	bool ParamInt::HasFail()
	{
		return !!this->failVal;
	}

	bool ParamInt::SetValue(int value, ValTy ty)
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

	bool ParamInt::GetValue(int& value, ValTy ty)
	{
		switch (ty)
		{
		case ValTy::Current:
			value = this->curVal;
			return true;

		case ValTy::Default:
			if (!this->defVal)
				return false;

			value = this->defVal.get();
			return true;

		case ValTy::Fail:
			if (!this->failVal)
				return false;

			value = this->failVal.get();
			return true;

		case ValTy::Max:
			if (!this->maxVal)
				return false;

			value = this->maxVal.get();
			return true;

		case ValTy::Min:
			if (!this->minVal)
				return false;

			value = this->minVal.get();
			return true;
		}
		return false;
	}

	bool ParamInt::SetValue(float value, ValTy ty)
	{
		int iv = (int)value;
		switch (ty)
		{
		case ValTy::Current:
			this->curVal = iv;
			return true;

		case ValTy::Default:
			this->defVal = iv;
			return true;

		case ValTy::Fail:
			this->failVal = iv;
			return true;

		case ValTy::Max:
			this->maxVal = iv;
			return true;

		case ValTy::Min:
			this->minVal = iv;
			return true;
		}
		return false;
	}

	bool ParamInt::GetValue(float& value, ValTy ty)
	{
		switch (ty)
		{
		case ValTy::Current:
			value = (float)this->curVal;
			return true;

		case ValTy::Default:
			if (!this->defVal)
				return false;

			value = (float)this->defVal.get();
			return true;

		case ValTy::Fail:
			if (!failVal)
				return false;

			value = (float)this->failVal.get();
			return true;

		case ValTy::Max:
			if (!this->maxVal)
				return false;

			value = (float)this->maxVal.get();
			return true;

		case ValTy::Min:
			if (!this->minVal)
				return false;

			value = (float)this->minVal.get();
			return true;
		}

		return false;
	}

	bool ParamInt::SetValue(const std::string & value, ValTy ty)
	{
		try
		{
			int is = std::stoi(value);

			switch (ty)
			{
			case ValTy::Current:
				this->curVal = is;
				return true;

			case ValTy::Default:
				this->defVal = is;

			case ValTy::Fail:
				this->failVal = is;

			case ValTy::Max:
				this->maxVal = is;

			case ValTy::Min:
				this->minVal = is;
			}
		}
		catch (const std::exception&) {
			return false;
		}

		return false;
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

	bool ParamInt::SetValue(bool value, ValTy ty)
	{
		int ib = value ? 1 : 0;

		switch (ty)
		{
		case ValTy::Current:
			this->curVal = ib;
			return true;

		case ValTy::Default:
			this->defVal = ib;
			return true;

		case ValTy::Fail:
			this->failVal = ib;
			return true;

		case ValTy::Max:
			this->maxVal = ib;
			return true;

		case ValTy::Min:
			this->minVal = ib;
			return true;
		}
		return false;
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
			if (!this->failVal)
				return false;

			value = this->failVal.get() != 0;
			return true;

		case ValTy::Max:
			if (!this->maxVal)
				return false;

			value = this->maxVal.get() != 0;
			return true;

		case ValTy::Min:
			if (!this->minVal)
				return false;

			value = this->minVal.get() != 0;
			return true;
		}
		return false;
	}

	bool ParamInt::ResetToDefault()
	{
		if (!this->defVal)
			return false;

		this->curVal = this->defVal.get();
		return true;
	}

	ParamSPtr ParamInt::Clone() const
	{
		ParamInt* pi = new ParamInt(*this);
		return ParamSPtr(pi);
	}
}