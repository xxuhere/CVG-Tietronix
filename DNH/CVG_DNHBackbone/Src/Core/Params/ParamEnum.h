#pragma once

#include "Param.h"
#include "ParamString.h"
#include <vector>
#include <boost/optional.hpp>

namespace CVG 
{
	/// <summary>
	/// Param subclass to support enum values.
	/// </summary>
	class ParamEnum : public ParamString
	{
		// Unlike most Param* implementations, ParamEnum derives
		// off an existing implementation of ParamString

	private:
		// The set of allowed values for the enum.
		std::vector<std::string> allowedValues;

	protected:
		void ModifyJSONImpl(json& js) override;

	public:
		/// <summary>
		/// Constructor for the enum implementation of Param.
		/// </summary>
		ParamEnum(
			const std::string& id,
			const std::string& label,
			const std::string& category,
			const std::string& unit,
			const std::string& value,
			boost::optional<std::string> defVal,
			boost::optional<std::string> failVal,
			std::vector<std::string> allowedValues);

		/// <summary>
		/// Checks if a value is contained in the set of 
		/// allowed values.
		/// </summary>
		/// <param name="qval">The value to check.</param>
		/// <returns>True if qval is an alloed enum value.</returns>
		bool ContainsAllowed(const std::string& qval);

		bool SetValue(int value, ValTy ty) override;
		bool SetValue(float value, ValTy ty) override;
		bool SetValue(const std::string& value, ValTy ty) override;
		bool SetValue(bool value, ValTy ty) override;
	};
}