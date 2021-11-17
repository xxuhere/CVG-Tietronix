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
			const std::string& guid,
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

		/// <summary>
		/// From a JSON defining an equipment, extract the relevant fields.
		/// </summary>
		/// <param name="js">The JSON to parse.</param>
		/// <param name="guid">The GUID.</param>
		/// <param name="manufacturer">The manufacturer.</param>
		/// <param name="name">The equipment name.</param>
		/// <param name="purpose">The equipment purpose.</param>
		/// <param name="type">The equipment type.</param>
		/// <param name="hostname">The equipment hostname.</param>
		/// <param name="outParams">
		/// Output parameter to the JSON holding the equipment parameters, or nullptr
		/// if none was found.
		/// </param>
		/// <returns>
		/// True if all mandatory expected fields were found. Else false. Note that 
		/// the requirements of what's "mandatory" is different betwen the server and
		/// clients. In this case, it is in respect to client usage.
		/// </returns>
		static bool ParseEquipmentFields(
			const json& js,
			std::string& guid,
			std::string& manufacturer,
			std::string& name,
			std::string& purpose,
			std::string& type,
			std::string& hostname,
			const json** outParams);

		/// <summary>
		/// Extract the client data of an Equipment define.
		/// 
		/// An Equipment data consists of several expected JSON elements. These are considered
		/// core descriptive data to an Equipment. Every other member is considered arbitrary
		/// user data. It does not do anything functionally in the DNH, but is kept and sent
		/// with the rest of the Equipment data as metadata.
		/// </summary>
		/// <param name="jsDst">Where to transfer the found client data.</param>
		/// <param name="jsSrc">An Equipment JSON to search for client data from.</param>
		/// <returns>True if any user data was transferred. Else, false.</returns>
		static bool ExtractClientData(json& jsDst, const json& jsSrc);
	};

	typedef std::shared_ptr<Equipment> BaseEqSPtr;
}