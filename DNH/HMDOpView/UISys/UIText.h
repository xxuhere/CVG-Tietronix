#pragma once

#include <string>
#include "UIBase.h"
#include "../FontMgr.h"

/// <summary>
/// A UI implemention for simple text rendering.
/// </summary>
class UIText : public UIBase
{
private:

	/// <summary>
	/// Horizontal alignment of text in respect to
	/// the UI widget's rect.
	/// </summary>
	HTextAlign halign = HTextAlign::Middle;

	/// <summary>
	/// Vertical alignment of the text in respect to
	/// the UI widget's rect.
	/// 
	/// NOTE: Currently multi-line strings are not supported.
	/// </summary>
	VTextAlign valign = VTextAlign::Center;

	/// <summary>
	/// The cached width of how long the text would be when
	/// rendered with the object's font.
	/// </summary>
	float cachedHAdvance = 0.0f;

	/// <summary>
	/// The string literal to display.
	/// </summary>
	std::string text;

	/// <summary>
	/// The font to render with.
	/// </summary>
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