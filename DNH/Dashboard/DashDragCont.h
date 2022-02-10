#pragma once

#include <Equipment.h>
#include "CamChannel.h"


/// <summary>
/// The dashboard can have multiple things dragged into them from
/// the inspector - Params and cameras. This container represents 
/// the possibility of any of those draggable things so interfaces 
/// only need 1 function for dragging and can branch with the contents
/// insode of DashDragCont when appropriate.
/// </summary>
class DashDragCont
{
public:
	enum class Type
	{
		None,
		Param,
		Cam
	};

	static const int DEF_PARAM_WIDTH = 20;
	static const int DEF_PARAM_HEIGHT = 5;
	//
	static const int DEF_CAM_WIDTH = 20;
	static const int DEF_CAM_HEIGHT = 15;

public:
	Type type = Type::None;

	CVG::ParamSPtr p;
	CamChannel camChan;

	// The default values for cached* variables control the
	// starting height and preview dimensions of dragged-in
	// items.
	int cachedWidth = DEF_PARAM_WIDTH;
	int cachedHeight = DEF_PARAM_HEIGHT;

	inline void Reset()
	{ this->type = Type::None; }

public:
	DashDragCont();
	DashDragCont(CVG::ParamSPtr p, int defWidth = DEF_PARAM_WIDTH, int defHeight = DEF_PARAM_HEIGHT);
	DashDragCont(const CamChannel & cc, int defWidth = DEF_CAM_WIDTH, int defHeight = DEF_CAM_HEIGHT);
};