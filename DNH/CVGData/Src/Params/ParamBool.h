#pragma once

#include "Param.h"
#include <boost/optional.hpp>

namespace CVG {

	/// <summary>
	/// Param subclass to support bool values.
	/// </summary>
	class ParamBool : public Param
	{
	private:
		bool curVal;
		boost::optional<bool> defVal; // default value
		boost::optional<bool> failVal; // fail-safe value

	protected:
		void ModifyJSONImpl(json& js) override;

	public:
		
		ParamBool(
			const std::string& id,
			const std::string& label,
			const std::string& category,
			const std::string& unit,
			bool value,
			boost::optional<bool> defVal,
			boost::optional<bool> failVal);

		// Override function. See base declaration of virtual functions 
		// in Param for documentation.
		std::string StrValue() override;
		bool HasMin() override;
		bool HasMax() override;
		bool HasFail() override;
		bool SetValue(int value, ValTy ty) override;
		bool GetValue(int& value, ValTy ty) override;
		bool SetValue(float value, ValTy ty) override;
		bool GetValue(float& value, ValTy ty) override;
		bool SetValue(const std::string& value, ValTy ty) override;
		bool GetValue(std::string& value, ValTy ty) override;
		bool SetValue(bool value, ValTy ty) override;
		bool GetValue(bool& value, ValTy ty) override;
		bool ResetToDefault() override;
		ParamSPtr Clone() const override;
	};
}