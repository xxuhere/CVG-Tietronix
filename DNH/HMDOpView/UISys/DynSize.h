#pragma once
#include "UIVec2.h"
#include "UIRect.h"

class DynSize
{
public:
	bool active = true;

	UIVec2 anchMin;
	UIVec2 anchMax;
	UIVec2 offsMin;
	UIVec2 offsMax;

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