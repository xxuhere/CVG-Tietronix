#include "IDashEle.h"

IDashEle::IDashEle(DashboardElementInst* eleInst)
{
	this->eleInst = eleInst;
}

IDashEle::~IDashEle()
{}

void IDashEle::OnRefreshInstance()
{
	this->Reattach();
}