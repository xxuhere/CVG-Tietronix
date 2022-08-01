#include "HardwareMgr.h"
#include "IHardware.h"
#include "../DicomUtils/DicomInjectorSet.h"

void HardwareMgr::ClearHardware(bool del)
{
	if(del)
	{
		for(IHardware* ihw : this->hws)
			delete ihw;
	}
	this->hws.clear();
	this->_initialized = false;
}

IHardware* HardwareMgr::GetFromName(const std::string& name)
{
	for(IHardware* ihw : this->hws)
	{
		if(ihw->HWName() == name)
			return ihw;
	}
	return nullptr;
}

bool HardwareMgr::Add(IHardware* hw)
{
	// No! If the initialize of equipment and the system already
	// happened, this throws a wrench in the state that was
	// previously validated.
	if(this->_initialized)
		return false;

	// We may also want to make sure it's not being readded.
	this->hws.push_back(hw);
	return true;
}

bool HardwareMgr::Remove(IHardware* hw, bool del)
{
	for(auto it = this->hws.begin(); it != this->hws.end(); ++it)
	{
		if(*it == hw)
		{
			if(del)
				delete *it;

			this->hws.erase(it);

			if(this->hws.size() == 0)
				this->_initialized = false;

			return true;
		}
	}

	return false;
}

bool HardwareMgr::Initialize(std::ostream& errStream)
{
	// Why are we attempting to initialize?
	if(this->_initialized)
		return false;

	bool ret = true;
	for(IHardware* ihw : this->hws)
	{
		if(!ihw->Initialize())
		{
			ret = false;
			errStream << "Initialization of " << ihw->HWName() << " returned in error!" << std::endl;
		}

		// If it implements an injector, auto-register it with the 
		// singleton injector set.
		DicomInjector* dji = ihw->GetInjector();
		if(dji)
		{
			DicomInjectorSet& dicomInjSet = DicomInjectorSet::GetSingleton();
			dicomInjSet.AddInjectorRef(dji);
		}
	}
	// _initialize is somewhat a misnomer, because validation
	// also needs to happen for it to be set true, which is 
	// expected to happen soon after a successful Initialize().

	return ret;
}

bool HardwareMgr::Validate(std::ostream& errStream)
{
	if(this->_initialized)
		return true;

	bool ret = true;
	for(IHardware* ihw : this->hws)
	{
		if(!ihw->Initialize())
		{
			ret = false;
			errStream << "Validation of " << ihw->HWName() << " returns in error!" << std::endl;
		}
	}
	this->_initialized = true;
	return ret;
}

HardwareMgr::~HardwareMgr()
{
	this->ClearHardware(true);
}