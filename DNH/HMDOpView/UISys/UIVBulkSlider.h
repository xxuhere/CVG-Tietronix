#pragma once
#include "UIBase.h"
#include "../FontMgr.h"
class UIVBulkSlider : public UIBase
{
public:
	float minVal;
	float maxVal;
	float curVal;
	float barHeightPx = 40.0f;
	FontWU fontMeter;

public:

	UIVBulkSlider(UIBase* parent, int idx, float min, float max, float cur, const UIRect& r);

	void HandleMouseDrag(const UIVec2& pos, int button) override;
	void HandleMouseDown(const UIVec2& pos, int button) override;
	void HandleMouseUp(const UIVec2& pos, int button) override;

	bool Render() override;
};