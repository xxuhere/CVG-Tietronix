#pragma once
#include "UIVec2.h"
#include "UIRect.h"

/// <summary>
/// A container holding variables for a dynamic UI resizing system.
/// The variables will reference landmarks on the parent UIBase or
/// offsets.
/// </summary>
class DynSize
{
public:
	/// <summary>
	/// Can be set to false to allow ignoring using the DynSize.
	/// </summary>
	bool active = true;

	//////////////////////////////////////////////////
	//
	//		ANCHORS
	//
	//////////////////////////////////////////////////
	// Anchors are location in the parents rect, represented by a 
	// horizontal and vertical value that go between [0.0, 1.0].

	/// <summary>
	/// The top left anchor.
	/// </summary>
	UIVec2 anchMin;

	/// <summary>
	/// The bottom right anchor.
	/// </summary>
	UIVec2 anchMax;

	//////////////////////////////////////////////////
	//
	//		OFFSETS
	//
	//////////////////////////////////////////////////
	// Offsets are vectors that are added to anchors to specify a final
	// corner of the rect. These will be in pixel values.

	/// <summary>
	/// The offset to anchMin.
	/// </summary>
	UIVec2 offsMin;

	/// <summary>
	/// The offset to anchMax.
	/// </summary>
	UIVec2 offsMax;

	//////////////////////////////////////////////////

public:
	DynSize();
	DynSize(const UIVec2& anchMin, const UIVec2& anchMax, const UIVec2& offsMin, const UIVec2& offsMax);

	DynSize& Set(const UIVec2& anchMin, const UIVec2& anchMax, const UIVec2& offsMin, const UIVec2& offsMax);
	DynSize& SetAnchors(const UIVec2& anchMin, const UIVec2& anchMax);
	DynSize& SetOffsets(const UIVec2& offsMin, const UIVec2& offsMax);
	DynSize& SetAnchors(float minPerX, float minPerY, float maxPerX, float maxPerY);
	DynSize& SetOffsets(float minPxX, float minPxY, float maxPxX, float maxPxY);
	DynSize& ZeroOffsets();
	DynSize& AnchorsAll();
	DynSize& AnchorsTop();
	DynSize& AnchorsBot();
	DynSize& AnchorsLeft();
	DynSize& AnchorsRight();
};