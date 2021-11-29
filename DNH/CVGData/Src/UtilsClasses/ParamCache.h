#pragma once
#include <string>
#include <memory>
#include <map>
#include <set>
#include "../Params/Param.h"

namespace CVG
{
	class ParamCache;
	typedef std::shared_ptr<ParamCache> ParamCacheSPtr;

	/// <summary>
	/// A collection of Params.
	/// 
	/// For now Equipments will have their own Params, but other
	/// things may choose to use them, and use them in a way 
	/// they're grouped together. This utility class can provide
	/// a unified way to handle that.
	/// </summary>
	class ParamCache
	{
	public:
		class iterator
		{
			friend Param;
		private:
			std::map<std::string, ParamSPtr>::iterator it;
		public:
			iterator(std::map<std::string, ParamSPtr>::iterator it);

			// Iterator implementation functions to allow ranged for 
			// loops. Other iterator functions are ignored.
			ParamSPtr operator *();
			void operator++();
			void operator--();
			bool operator == (const iterator& itOther) const;
			bool operator != (const iterator& itOther) const;
		};
		friend iterator;

		class const_iterator
		{
			friend Param;
		private:
			std::map<std::string, ParamSPtr>::const_iterator it;
		public:
			const_iterator(std::map<std::string, ParamSPtr>::const_iterator it);

			// Iterator implementation functions to allow ranged for 
			// loops. Other iterator functions are ignored.
			const ParamSPtr operator *();
			void operator++();
			void operator--();
			bool operator == (const const_iterator& itOther) const;
			bool operator != (const const_iterator& itOther) const;
		};
		friend const_iterator;

	private:
		/// <summary>
		/// The collection of the Params being managed.
		/// 
		/// The dictionary key is the Params' id.
		/// </summary>
		std::map<std::string, ParamSPtr> params;

	public:

		ParamCache();

		ParamCache(std::vector<ParamSPtr> params);

		inline size_t Size() const
		{ return this->params.size(); }

		/// <summary>
		/// Parse a Param JSON definition and add it to the 
		/// ParamCache.
		/// </summary>
		/// <param name="jsparam">The Param JSON definition.</param>
		/// <param name="error">
		/// If the return value is nullptr, this output string can be 
		/// checked for a human readable reason. If the return value
		/// is not nullptr, the value can be ignored.
		/// </param>
		/// <returns>
		/// A reference to the create Param. nullptr if the parse and
		/// creation failed. This can either be because the json could
		/// not be correctly parsed, or because a Param with the same
		/// id already exists.
		/// </returns>
		ParamSPtr ParseParamJSON(const json& jsparam, std::string & error);

		/// <summary>
		/// Reset all variables in the ParamCache to their
		/// default values.
		/// </summary>
		/// <param name="removeNoDefs">
		/// If true, and a Param is found to not have a default
		/// value, it will be removed.
		/// </param>
		/// <param name="modified"> A set of modified Param ids. Only Params
		/// with default values will be modified from a reset. If this
		/// is not needed, nullptr can be passed in.
		/// </param>
		/// <param name="submitted">
		/// A set of Param ids that should be event-submitted.
		/// </param>
		void Reset(
			bool removeNoDefs, 
			std::set<std::string> * outModified,
			std::set<std::string>* outRemoved,
			std::set<std::string>* submitted);

		/// <summary>
		/// Clear all variables in the ParamCache.
		/// </summary>
		void Clear();

		/// <summary>
		/// Check if a Param ID is contained.
		/// </summary>
		/// <param name="id">The Param id to query.</param>
		/// <returns>
		/// True if a Param with the id was found in the ParamCache.
		/// Else, false.
		/// </returns>
		bool Contains(const std::string&  id);

		/// <summary>
		/// Get a Param by id.
		/// </summary>
		/// <param name="id">The id to retrieve.</param>
		/// <returns>
		/// The found Param matching the id. nullptr if a match was not
		/// found.</returns>
		ParamSPtr Get(const std::string&  id);

		inline ParamSPtr operator[](const std::string& id)
		{ return this->Get(id); }

		/// <summary>
		/// Set the value of a Param to a JSON value.
		/// 
		/// Note that this is a JSON "value", this should not be
		/// confused with a JSON Param definition.
		/// </summary>
		/// <param name="paramid">The Param id to set.</param>
		/// <param name="jsVal">The JSON value to set the Param to.</param>
		/// <param name="createifmissing">
		/// If the Param was not found, automatically create the 
		/// Param with the requested ID and set its value.
		/// </param>
		/// <returns>
		/// True if a Param was found/created and successfully set.
		/// Else, false.
		/// </returns>
		SetRet Set(std::string& paramid, const json& jsVal, bool createifmissing = false);

		// Set() functions for each individual data types. Note that 
		// strings and enums will use the same "set" function. 
		//
		// And if createifmissing is used, there's  no way to create 
		// an enum since that requires extra information (the possible
		// entries) that it wouldn't have.
		SetRet Set(std::string& paramid, int iVal,					bool createifmissing = false);
		SetRet Set(std::string& paramid, float fVal,				bool createifmissing = false);
		SetRet Set(std::string& paramid, const std::string& sVal,	bool createifmissing = false);
		SetRet Set(std::string& paramid, bool bVal,					bool createifmissing = false);

		iterator begin()
		{
			return iterator(this->params.begin());
		}

		iterator end()
		{
			return iterator(this->params.end());
		}

		const_iterator begin() const
		{
			return const_iterator(this->cbegin());
		}

		const_iterator end() const
		{
			return const_iterator(this->cend());
		}

		const_iterator cbegin() const
		{
			return const_iterator(this->params.cbegin());
		}

		const_iterator cend() const
		{
			return const_iterator(this->params.cend());
		}

		/// <summary>
		/// Get a JSON object that maps Param IDs to their values.
		/// 
		/// Example of the return value:
		/// {
		///		"bool_param" : true,
		///		"otherparamid" : 45,
		///		"anotherparam" : "etc"
		/// }
		/// </summary>
		/// <returns>
		/// The JSON object with a id<->value relationship.
		/// </returns>
		json JSONValueObject() const;

		/// <summary>
		/// Retrive a JSON array containing all the Param definitions
		/// in the ParamCache.
		/// </summary>
		/// <returns>The JSON array with a detailed description of all 
		/// Params contained in the ParamCache.</returns>
		json JSONDefinitionsArray() const;

		/// <summary>
		/// Create a copy of the ParamCache.
		/// 
		/// The created ParamCache will contain a reference to the same Params.
		/// This can be used as a threadsafe copy to iterating the list of items.
		/// (No to be confused with threadsafe access to the actual Parameters).
		/// 
		/// The actual cloning process isn't thread safe. It should either be performed
		/// at a time that's ensured no other threads will write to it, or be
		/// guarded in a mutex.
		/// </summary>
		/// <returns>The cloned ParamCacheSPtr.</returns>
		ParamCacheSPtr Clone();

		/// <summary>
		/// Creates a deep copy of this object. This means the items contained
		/// within the new object's params will be clones and not references.
		/// </summary>
		/// <returns>The cloned ParamCacheSPtr with deep copies of the 
		/// individual parameters.</returns>
		ParamCacheSPtr DeepClone();
	};
}