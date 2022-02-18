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
	/// <summary>
	/// The types of content that can be dragged from the inspector
	/// into the canvas.
	/// </summary>
	/// <remarks>
	/// This may be a DRY violation because of how similar this
	/// is to Tile::Type.
	/// </remarks>
	enum class Type
	{
		/// <summary>
		/// Not a valid type. Used as an error code.
		/// </summary>
		None,

		/// <summary>
		/// An equipment parameter.
		/// </summary>
		Param,

		/// <summary>
		/// An equipment camera feed.
		/// </summary>
		Cam
	};

	/// <summary>
	/// Default width of a new parameter (in tile units) being dragged in.
	/// </summary>
	static const int DEF_PARAM_WIDTH = 20;

	/// <summary>
	/// Default height of a new parameter (in tile units) being dragged in.
	/// </summary>
	static const int DEF_PARAM_HEIGHT = 5;
	
	/// <summary>
	/// Default width of a new camera (in tile units) being dragged in.
	/// </summary>
	static const int DEF_CAM_WIDTH = 20;

	/// <summary>
	/// Default height of a new camera (in tile units) being dragged in.
	/// </summary>
	static const int DEF_CAM_HEIGHT = 15;

public:
	/// <summary>
	/// The type of inspector-to-canvas drag operation being performed.
	/// </summary>
	Type type = Type::None;

	/// <summary>
	/// The parameter being dragged.
	/// 
	/// Only valid if type == Type::Param.
	/// </summary>
	CVG::ParamSPtr p;

	/// <summary>
	/// The camera URI being dragged.
	/// 
	/// Only valid if type == Type::Cam.
	/// </summary>
	CamChannel camChan;

	// The default values for cached* variables control the
	// starting height and preview dimensions of dragged-in
	// items.
	int cachedWidth = DEF_PARAM_WIDTH;
	int cachedHeight = DEF_PARAM_HEIGHT;

	/// <summary>
	/// Reset the drag data. This can be used to invalidate 
	/// DashDragCont objects that are used as a drag and drop.
	/// </summary>
	inline void Reset()
	{ this->type = Type::None; }

public:
	DashDragCont();
	DashDragCont(CVG::ParamSPtr p, int defWidth = DEF_PARAM_WIDTH, int defHeight = DEF_PARAM_HEIGHT);
	DashDragCont(const CamChannel & cc, int defWidth = DEF_CAM_WIDTH, int defHeight = DEF_CAM_HEIGHT);
};