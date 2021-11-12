#pragma once

#include "Types/DataType.h"
#include "Types/EqType.h"
#include "Params/Param.h"
#include "UtilsClasses/ParamCache.h"

namespace CVG
{
	/// <summary>
	/// Representation of an Equipment.
	/// </summary>
	class Equipment
	{
		typedef ParamCache::iterator iterator;
		typedef ParamCache::const_iterator const_iterator;
	protected:
		// The name of the Equipment.
		std::string name;

		// The name of the Equipment manufacturer. This can be 
		// empty if the concept of a manufactuerer does not apply.
		// (Such as for spectator connections)
		std::string manufacturer;

		/// <summary>
		/// Specify the Equipment's purpose in a device stack.
		/// </summary>
		std::string purpose;

		// The GUID of the equipment. This value is assigned by the
		// DNH during runtime, and will be unique for ALL Equipment.
		std::string guid;

		// The hostname of the equipment. If the host wants to provide
		// its local network ID so other things can directly connect
		// to it.
		std::string hostname;

		// The type of equipment.
		EQType equipmentType;

		// The parameters.
		ParamCache paramCache;

		// Extra misc data. Since we don't know what it is, we'll
		// leave it as JSON since that's a good format for holding
		// arbitrary typed data hierarchies.
		json clientData;

	public:
		Equipment(
			const std::string & name, 
			const std::string & manufacturer,
			const std::string & purpose,
			const std::string& hostname,
			EQType type,
			std::vector<ParamSPtr> params,
			json clientData);

		std::string GUID() const 
		{ return this->guid; }

		std::string Purpose() const 
		{ return this->purpose; }

		inline std::string TypeStr() const
		{ return ConvertToString(this->equipmentType); };

		inline EQType Type() const 
		{ return this->equipmentType; }

		inline std::string Name()const
		{ return this->name; }

		/// <summary>
		/// Parameter iterator to allow iteration and use of 
		/// ranged-for loops.
		/// </summary>
		/// <returns>The starting iterator for contained Params.</returns>
		iterator begin();

		/// <summary>
		/// Parameter iterator to allow iteration and use of 
		/// ranged-for loops.
		/// </summary>
		/// <returns>The ending iterator for contained Params.</returns>
		iterator end();

		/// <summary>
		/// Find a parameter in the Equipment.
		/// </summary>
		/// <param name="id">The id of the Parameter.</param>
		/// <returns>
		/// A reference to the Param in the Equipment.
		/// nullptr will be found if a Param with a matching id was 
		/// not found.
		/// </returns>
		ParamSPtr GetParam(const std::string& id);

		/// <summary>
		/// Get an in-depth description of the Equipment as a JSON
		/// representation.
		/// 
		/// This includes the introspection and current value of the 
		/// parameters.
		/// </summary>
		/// <returns>
		/// Details, and the current state, of the Equipment as a 
		/// JSON object.
		/// </returns>
		virtual json GetJSON() const;

		/// <summary>
		/// Get an short description of the Equipment as a JSON represention.
		/// </summary>
		/// <returns>The equipment's name and GUID as a JSON object.</returns>
		virtual json GetJSONSummary() const;

	public:
		/// <summary>
		/// Create the basic JSON object for an Equipment definition.
		/// 
		/// Some things that aren't actually Equipment may wish to 
		/// defined themselves in JSON the same way Equipments do.
		/// </summary>
		/// <param name="name">The Equipment type.</param>
		/// <param name="guid">The Equipment guid.</param>
		/// <param name="type">The Equipment type.</param>
		/// <param name="purpose">The Equipment purpose.</param>
		/// <returns>The barebones Equipment definition.</returns>
		static json EquipmentJSONTemplate(
			const std::string& name,
			const std::string& guid,
			EQType eqTy,
			const std::string& purpose);
	};

	typedef std::shared_ptr<Equipment> BaseEqSPtr;
}