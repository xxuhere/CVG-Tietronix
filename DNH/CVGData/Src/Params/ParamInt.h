#pragma once

#include "Param.h"
#include <boost/optional.hpp>

namespace CVG {

	/// <summary>
	/// Implementation of the Param variant for int datatypes.
	/// </summary>
	class ParamInt : public Param
	{
	private:
		int curVal;

		boost::optional<int> defVal;
		boost::optional<int> failVal;
		boost::optional<int> minVal;
		boost::optional<int> maxVal;

	protected:
		void ModifyJSONImpl(json& js) override;

	public:
		/// <summary>
		/// Constructor for the int implementation of Param.
		/// </summary>
		ParamInt(
			const std::string& id,
			const std::string& label,
			const std::string& category,
			const std::string& unit,
			int value,
			boost::optional<int> defVal,
			boost::optional<int> failVal,
			boost::optional<int> minVal,
			boost::optional<int> maxVal);

		// Override function. See base declaration of virtual functions 
		// in Param for documentation.
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