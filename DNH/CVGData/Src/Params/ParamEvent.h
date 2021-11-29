#pragma once

#include "Param.h"

namespace CVG
{
	/// <summary>
	/// Param subclass to represent triggerable events.
	/// 
	/// This isn't really a variable type, but are action that are
	/// piggy-backing off the Param system to 
	/// - Belong to equipment
	/// - Have names
	/// - Use the JSON API
	/// - Be listed in introspection enumerations.
	/// 
	/// No value is ever kept or set in the Param, but the API can send
	/// the value "submit" to triggert the event. It uses the hardcoded
	/// phrase "submit" to ensure the event was intentionally triggered.
	/// </summary>
	class ParamEvent : public Param
	{
		// Send a submit when a reset occurs.
		bool triggerOnReset;

		// Send a submit when a fail is detected.
		bool triggerOnFailsafe;

	protected:
		void ModifyJSONImpl(json& js) override;

	public:
		ParamEvent(
			const std::string& id,
			const std::string& label,
			const std::string& category,
			const std::string& unit,
			bool trigReset,
			bool trigFailSafe);

		std::string StrValue() override;
		bool HasMin() override;
		bool HasMax() override;
		bool HasFail() override;
		SetRet SetValue(int value, ValTy ty) override;
		bool GetValue(int& value, ValTy ty) override;
		SetRet SetValue(float value, ValTy ty) override;
		bool GetValue(float& value, ValTy ty) override;
		SetRet SetValue(const std::string& value, ValTy ty) override;
		bool GetValue(std::string& value, ValTy ty) override;
		SetRet SetValue(bool value, ValTy ty) override;
		bool GetValue(bool& value, ValTy ty) override;
		SetRet ResetToDefault() override;
		ParamSPtr Clone() const override;
	};
}