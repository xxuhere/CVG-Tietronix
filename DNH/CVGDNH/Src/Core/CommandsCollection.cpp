#include "CommandsCollections.h"
#include <algorithm>
#include <boost/process.hpp>

namespace CVG
{
	CommandsCollection::CommandsCollection(const std::string& name)
	{
		this->name = name;
	}

	void CommandsCollection::Execute()
	{
		for(const std::string & strCmd : this->commands)
			boost::process::system(strCmd);
	}

	void CommandsCollection::AddCommand(const std::string& cmd)
	{
		this->commands.push_back(cmd);
	}

	void CommandsCollection::Clear()
	{
		this->commands.clear();
	}
}