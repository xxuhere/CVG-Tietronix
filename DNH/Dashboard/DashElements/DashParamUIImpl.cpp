#include "DashParamUIImpl.h"

DashParamUIImpl::DashParamUIImpl(DashboardElementInst* eleInst)
{
	this->eleInst = eleInst;
}

DashParamUIImpl::~DashParamUIImpl()
{}

void DashParamUIImpl::OnRefreshInstance()
{
	this->Reattach();
}