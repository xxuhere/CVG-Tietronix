#include "ParamEvent.h"
namespace CVG
{ 
	ParamEvent::ParamEvent(
		const std::string& id,
		const std::string& label,
		const std::string& category,
		const std::string& unit,
		bool trigReset,
		bool trigFailSafe)
		: Param(id, label, category, unit, DataType::Event)
	{
		this->triggerOnReset	= trigReset;
		this->triggerOnFailsafe = trigFailSafe;
	}

	void ParamEvent::ModifyJSONImpl(json& js)
	{
		js["current"] = "";

		if(this->triggerOnReset)
			js["default"] = true;

		if (this->triggerOnFailsafe)
			js["fail"] = true;
	}

	// Events never actually hold any current value, only
	// submits on a true or "submit"
	std::string ParamEvent::StrValue() 
	{ return ""; }

	bool ParamEvent::HasMin() 
	{ return false; }

	bool ParamEvent::HasMax() 
	{ return false; }

	bool ParamEvent::HasFail() 
	{ return this->triggerOnFailsafe; }

	SetRet ParamEvent::SetValue(int value, ValTy ty) 
	{
		// Int to event conversion undefined
		return SetRet::Invalid;
	}

	bool ParamEvent::GetValue(int& value, ValTy ty) 
	{
		return false;
	}

	SetRet ParamEvent::SetValue(float value, ValTy ty) 
	{
		// Float to event conversion undefined.
		return SetRet::Invalid;
	}

	bool ParamEvent::GetValue(float& value, ValTy ty) 
	{
		return false;
	}

	SetRet ParamEvent::SetValue(const std::string& value, ValTy ty) 
	{
		switch(ty)
		{
		case ValTy::Current:
			if(value == "submit")
				return SetRet::Submit;
			else
				return SetRet::Success;

		case ValTy::Default:
			this->triggerOnReset = (value == "submit");
			return SetRet::Success;

		case ValTy::Fail:
			this->triggerOnFailsafe = (value == "submit");
			return SetRet::Success;
		}

		return SetRet::Invalid;
	}

	bool ParamEvent::GetValue(std::string& value, ValTy ty) 
	{
		switch(ty)
		{
		case ValTy::Current:
			value = "";
			return true;

		case ValTy::Default:
			if(this->triggerOnReset)
				value = "submit";
			else
				value = "";
			return true;

		case ValTy::Fail:
			if(this->triggerOnFailsafe)
				value = "submit";
			else
				value = "";
			return true;

		}
		return false;
	}

	SetRet ParamEvent::SetValue(bool value, ValTy ty) 
	{
		switch(ty)
		{
		case ValTy::Current:
			if(value)
				return SetRet::Submit;
			return SetRet::Success;

		case ValTy::Default:
			this->triggerOnReset = value;
			return SetRet::Success;

		case ValTy::Fail:
			this->triggerOnFailsafe = value;
			return SetRet::Success;
		}

		return SetRet::Invalid;
	}

	bool ParamEvent::GetValue(bool& value, ValTy ty) 
	{
		switch(ty)
		{
		case ValTy::Current:
			return true;

		case ValTy::Default:
			return true;

		default:
			return false;
		}
		return true;
	}

	SetRet ParamEvent::ResetToDefault()
	{
		if(this->triggerOnReset)
			return SetRet::Submit;

		return SetRet::Success;
	}

	ParamSPtr ParamEvent::Clone() const
	{
		ParamEvent* pe = new ParamEvent(*this);
		return ParamSPtr(pe);
	}
}