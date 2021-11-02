#include "boost/optional.hpp"

#include "ParamUtils.h"

#include "../ParseUtils.h"

namespace CVG
{
	ParamSPtr ParamUtils::Parse(const json& js, std::string& error)
	{
		if (!js.is_object())
		{
			error = "Attempting to parse Param that is not an object.";
			return nullptr;
		}

		std::string id;
		if (!ParseUtils::ExtractJSONString(js, "id", id))
		{
			error = "Encountered Param missing id.";
			return nullptr;
		}

		std::string strtype;
		if (!ParseUtils::ExtractJSONString(js, "type", strtype))
		{
			error = "Encountered " + id + " Param missing type.";
			return nullptr;
		}

		DataType dty = ConvertToDataType(strtype);
		if (dty == DataType::Unknown)
		{
			error = "Encounter " + id + " Param with unknown data type.";
			return nullptr;
		}

		std::string strunit;
		ParseUtils::ExtractJSONString(js, "unit", strunit);

		std::string label;
		if (!ParseUtils::ExtractJSONString(js, "label", label))
		{
			// If a label isn't given, the best we can do is 
			// show (in the GUI) the value of the id. While they
			// will often be similar, remember the id isn't 
			// meant for human readability so it's a fallback.
			label = id;
		}

		std::string category;
		ParseUtils::ExtractJSONString(js, "category", category);

		switch (dty)
		{
		case DataType::Bool:
			return ParamUtils::ParseBool(js, id, label, category, strunit, error);

		case DataType::Enum:
			return ParamUtils::ParseEnum(js, id, label, category, strunit, error);

		case DataType::Float:
			return ParamUtils::ParseFloat(js, id, label, category, strunit, error);

		case DataType::Int:
			return ParamUtils::ParseInt(js, id, label, category, strunit, error);

		case DataType::String:
			return ParamUtils::ParseString(js, id, label, category, strunit, error);

		default:
			// This would be an error on our side if there was a 
			// DataType we were able to parse the enum for, but 
			// not create an instance of.
			//
			// It's in caps so it doesn't get confused with the
			// unknown error.
			error = "Encountered " + id + " Param with UNHANDLED data type.";
			return nullptr;
		}
		
	}

	// There are a lot of overlaps between these Parse* functions. And they 
	// fall into two categories,
	// - Items that have an optional min max.
	// - Items that don't support min max.
	// We may be able to consolidate a lot of the logic by having two functions
	// that unify these categories it (macros or templates), but for now we'll 
	// work with duplicates for each data type for the sake of development 
	// simplicity.
	//
	// Any attempt to consolidate and unify duplicate parts of these similar
	// functions is welcome, as long as it doesn't change their behaviour.
	// (wleu 10/22/2021)

	ParamSPtr ParamUtils::ParseInt(
		const json& js,
		const std::string& id,
		const std::string& label,
		const std::string& category,
		const std::string& unit,
		std::string& error)
	{
		struct $_anon
		{
			static boost::optional<int> PullInt(const json& js, const std::string& name)
			{
				if (!js.contains(name))
					return boost::none;

				json jsmem = js[name];
				if (!jsmem.is_number_integer())
					return boost::none;

				return (int)jsmem;
			}
		};

		boost::optional<int> vcur = $_anon::PullInt(js, "current");
		boost::optional<int> vdef = $_anon::PullInt(js, "default");
		if (!vdef && !vcur)
		{
			error = "Param int " + id + ", must contain either a default or current value.";
			return nullptr;
		}
		if (!vcur)
			vcur = vdef;


		boost::optional<int> vmin = $_anon::PullInt(js, "min");
		boost::optional<int> vmax = $_anon::PullInt(js, "max");
		boost::optional<int> vfail = $_anon::PullInt(js, "fail");

		ParamInt* pi = new ParamInt(id, label, category, unit, vcur.get(), vdef, vfail, vmin, vmax);
		ParamSPtr ret = ParamSPtr(pi);
		return ret;
	}

	ParamSPtr ParamUtils::ParseString(
		const json& js,
		const std::string& id,
		const std::string& label,
		const std::string& category,
		const std::string& unit,
		std::string& error)
	{
		struct $_anon
		{
			// We need this function because there is a difference
			// between a string that's not in the JSON object, or
			// a string that's there, but empty.
			static boost::optional<std::string> PullString(const json& js, const std::string& name)
			{
				if (!js.contains(name) || !js[name].is_string())
					return boost::none;

				return js[name];
			}
		};

		boost::optional<std::string> vcur = $_anon::PullString(js, "current");
		boost::optional<std::string> vdef = $_anon::PullString(js, "default");
		if (!vdef && !vcur)
		{
			error = "Param string " + id + ", must have either a default or current value.";
			return nullptr;
		}
		if (!vcur)
			vcur = vdef;

		boost::optional<std::string> vfail = $_anon::PullString(js, "fail");

		ParamString* ps = new ParamString(id, label, category, unit, vcur.get(), vdef, vfail);
		ParamSPtr ret = ParamSPtr(ps);
		return ret;
	}

	ParamSPtr ParamUtils::ParseBool(
		const json& js,
		const std::string& id,
		const std::string& label,
		const std::string& category,
		const std::string& unit,
		std::string& error)
	{
		struct $_anon
		{
			static boost::optional<bool> PullBool(const json& js, const std::string& name)
			{
				if (!js.contains(name))
					return boost::none;

				json jsmem = js[name];
				if (jsmem.is_boolean())
					return (bool)jsmem;

				if (jsmem.is_number_integer())
					return (int)jsmem != 0;

				if (jsmem.is_number_float())
					return (float)jsmem != 0.0f;

				return boost::none;
			}
		};
		boost::optional<bool> vcur = $_anon::PullBool(js, "current");
		boost::optional<bool> vdef = $_anon::PullBool(js, "default");

		if (!vdef && !vcur)
		{
			error = "Param bool " + id + " must have either a current or default value.";
			return nullptr;
		}
		if (!vcur)
			vcur = vdef;

		boost::optional<bool> vfail = $_anon::PullBool(js, "fail");

		ParamBool* pb = new ParamBool(id, label, category, unit, vcur.get(), vdef, vfail);
		ParamSPtr ret = ParamSPtr(pb);
		return ret;
	}

	ParamSPtr ParamUtils::ParseEnum(
		const json& js,
		const std::string& id,
		const std::string& label,
		const std::string& category,
		const std::string& unit,
		std::string& error)
	{
		// This is almost a complete duplicate of ParseString().
		struct $_anon
		{
			static boost::optional<std::string> PullString(const json& js, const std::string& name)
			{
				if (!js.contains(name) || !js[name].is_string())
					return boost::none;

				return js[name];
			}
		};

		boost::optional<std::string> vcur = $_anon::PullString(js, "current");
		boost::optional<std::string> vdef = $_anon::PullString(js, "default");
		if (!vdef && !vcur)
		{
			error = "Param enum " + id + ", must have either a default or current value.";
			return nullptr;
		}
		if (!vcur)
			vcur = vdef;

		// The only big different from ParseString is the extraction of the 
		// possible values set.
		//////////////////////////////////////////////////
		//
		if (!js.contains("possible"))
		{
			error = "Param enum" + id + " is missing the possible values set";
			return nullptr;
			
		}
		json jspos = js["possible"];
		if (!jspos.is_array())
		{
			error = "Param enum" + id + ", possible values must be an array.";
			return nullptr;
		}
		std::vector<std::string> possibleVals;
		for (json jp : jspos)
			possibleVals.push_back(jp);
		//
		//////////////////////////////////////////////////

		boost::optional<std::string> vfail = $_anon::PullString(js, "fail");

		ParamEnum* pe = new ParamEnum(id, label, category, unit, vcur.get(), vdef, vfail, possibleVals);
		ParamSPtr ret = ParamSPtr(pe);
		return ret;
	}

	ParamSPtr ParamUtils::ParseFloat(
		const json& js,
		const std::string& id,
		const std::string& label,
		const std::string& category,
		const std::string& unit,
		std::string& error)
	{
		struct $_anon
		{
			static boost::optional<float> PullFloat(const json& js, const std::string& name)
			{
				if (!js.contains(name))
					return boost::none;

				json jsmem = js[name];
				if (!jsmem.is_number())
					return boost::none;

				return (float)jsmem;
			}
		};

		boost::optional<float> vcur = $_anon::PullFloat(js, "current");
		boost::optional<float> vdef = $_anon::PullFloat(js, "default");
		if (!vdef && !vcur)
		{
			error = "Param float " + id + ", must have either a default or current value.";
			return nullptr;
		}
		if(!vcur)
			vcur = vdef;


		boost::optional<float> vmin = $_anon::PullFloat(js, "min");
		boost::optional<float> vmax = $_anon::PullFloat(js, "max");
		boost::optional<float> vfail = $_anon::PullFloat(js, "fail");

		ParamFloat* pf = new ParamFloat(id, label, category, unit, vcur.get(), vdef, vfail, vmin, vmax);
		ParamSPtr ret = ParamSPtr(pf);
		return ret;
	}
}