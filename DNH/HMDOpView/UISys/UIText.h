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

	void SetHAlignment(HTextAlign halign);
	void SetVAlignment(VTextAlign valign);
	void SetTextAlignment(HTextAlign halign, VTextAlign valign);

	// Trying a style, don't know if we'll keep these
	// (wleu 05/25/2022)
	UIText& operator << (HTextAlign halign);
	UIText& operator << (VTextAlign valign);

	void SetText(const std::string& text);

	bool Render() override;
};