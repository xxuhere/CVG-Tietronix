#pragma once
#include "UIBase.h"

class UIHSlider : public UIBase
{
public:
	float minVal;
	float maxVal;
	float curVal;
public:

	UIHSlider(UIBase* parent, int idx, float min, float max, float cur, const UIRect& r);

	bool Render() override;
};