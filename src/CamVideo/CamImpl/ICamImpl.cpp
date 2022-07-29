#include "ICamImpl.h"
#include "../../Utils/cvgOptions.h"

bool ICamImpl::Initialize()
{
	if(this->hasInitialized)
		return false;

	this->hasInitialized = this->InitializeImpl();
	return this->hasInitialized;
}

bool ICamImpl::Shutdown()
{
	if(!this->hasInitialized || this->hasShutdown)
		return false;

	bool ret = this->ShutdownImpl();
	this->hasShutdown = true;

	return ret;
}

bool ICamImpl::Activate()
{
	// For now it's just a direct delegation to the 
	// subclass implementation.
	return this->ActivateImpl();
}

bool ICamImpl::Deactivate()
{
	// For now it's just a direct delegation to the 
	// subclass implementation.
	return this->DeactivateImpl();
}

ICamImpl::~ICamImpl()
{
	this->Shutdown();
}

bool ICamImpl::PullOptions(const cvgCamFeedLocs& opts)
{
	this->prefWidth		= opts.streamWidth;
	this->prefHeight	= opts.streamHeight;
	return true;
}