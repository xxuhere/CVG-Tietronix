#include "DashDragCont.h"

DashDragCont::DashDragCont()
{
	this->type = Type::None;
}

DashDragCont::DashDragCont(CVG::ParamSPtr p, int defWidth, int defHeight)
{
	this->type = Type::Param;
	this->p = p;

	this->cachedWidth = defWidth;
	this->cachedHeight = defHeight;
}

DashDragCont::DashDragCont(const CamChannel & cc, int defWidth, int defHeight)
{
	this->type = Type::Cam;
	this->camChan = cc;

	this->cachedWidth = defWidth;
	this->cachedHeight = defHeight;
}