#pragma once

#include "Param.h"
#include <boost/optional.hpp>

namespace CVG 
{

	/// <summary>
	/// Param subclass to support string values.
	/// </summary>
	class ParamString : public Param
	{
	protected:
		std::string curVal;

		boost::optional <std::string> defVal;
		boost::optional<std::string> failVal;

		std::string unit;

	protected:
		void ModifyJSONImpl(json& js) override;

	public:

		/// <summary>
		/// Constructor for the string implementation of Param.
		/// </summary>
		ParamString(
			const std::string& id,
			const std::string& label,
			const std::string& category,
			const std::string& unit,
			const std::string& value,
			boost::optional<std::string> defVal,
			boost::optional<std::string> failVal);

		/// <summary>
		/// Constructor for subtypes that derive off of ParamString.
		/// </summary>
		ParamString(
			const std::string& id,
			const std::string& label,
			const std::string& category,
			const std::string& unit,
			const std::string& value,
			boost::optional<std::string> defVal,
			boost::optional<std::string> failVal,
			DataType ty);

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