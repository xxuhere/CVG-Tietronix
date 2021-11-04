#pragma once
#include <string>
#include <vector>
namespace CVG
{
	/// <summary>
	/// A collection of system commands (i.e., shell or command 
	/// console) to group together and run in batch.
	/// </summary>
	class CommandsCollection
	{
		// There's nothing sensitive in here that would require
		// any members to have protected encapsulation access.
	public:
		/// <summary>
		/// The name of the command. Not a functional variable - 
		/// metadata.
		/// </summary>
		std::string name;

		/// <summary>
		/// The system commands.
		/// </summary>
		std::vector<std::string> commands;

	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="name">Name of the collection.</param>
		CommandsCollection(const std::string & name);

		/// <summary>
		/// Execute all commands, at once, asyncronously,
		/// in the order (as they appear thin this->commands).
		/// </summary>
		void Execute();

		/// <summary>
		/// Add a command to the end of the commands list.
		/// </summary>
		/// <param name="cmd">The command to add.</param>
		void AddCommand(const std::string& cmd);

		/// <summary>
		/// Clear all commands.
		/// </summary>
		void Clear();
	};
}