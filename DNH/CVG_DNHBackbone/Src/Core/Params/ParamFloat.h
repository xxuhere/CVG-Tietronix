#pragma once

#include "Param.h"
#include "boost/optional.hpp"

namespace CVG 
{
	/// <summary>
	/// Param subclass to support float values.
	/// </summary>
	class ParamFloat : public Param
	{
	private:
		float curVal;

		boost::optional<float> defVal;
		boost::optional<float> failVal;
		boost::optional<float> minVal;
		boost::optional<float> maxVal;

	protected:
		void ModifyJSONImpl(json& js) override;

	public:
		/// <summary>
		/// Constructor for the float implementation of Param.
		/// </summary>
		ParamFloat(
			const std::string& id,
			const std::string& label,
			const std::string& category,
			const std::string& unit,
			float value,
			boost::optional<float> defVal,
			boost::optional<float> failVal,
			boost::optional<float> minVal,
			boost::optional<float> maxVal);

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
	};
}