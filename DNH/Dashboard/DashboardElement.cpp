#include "DashboardElement.h"

bool DashboardElement::DestroyUIImpl()
{
	if (this->uiImpl != nullptr)
		return false;

	this->uiImpl->Destroy();
	this->uiImpl = nullptr;
	this->gridOwner = nullptr;
	return true;
}