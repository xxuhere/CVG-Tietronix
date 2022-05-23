#pragma once

#include <string>
#include "UIBase.h"
#include "../FontMgr.h"

class UIText : public UIBase
{
private:
	HTextAlign halign = HTextAlign::Middle;
	VTextAlign valign = VTextAlign::Center;

	float cachedHAdvance = 0.0f;
	std::string text;
	FontWU fontHandle;

public:
	UIText(UIBase* parent, int idx, const std::string& text, int size, const UIRect& r);

	void SetText(const std::string& text);

	bool Render() override;
};