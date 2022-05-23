#pragma once
#include "UIBase.h"
#include <string>
#include "../TexObj.h"
#include "../FontMgr.h"
#include <initializer_list>

class UIButton : public UIBase
{
public:
	TexObj::SPtr image;

	std::string text;
	FontWU font;

	UIColor4 textColor;

private:
	void _InitializeColors();

public:
	UIButton(UIBase* parent, int idx, const UIRect& r, const std::string& filePath);
	UIButton(UIBase* parent, int idx, const UIRect& r, TexObj::SPtr img);
	UIButton(
		UIBase* parent,
		int idx, 
		const UIRect& r, 
		const std::string& text, 
		int size,
		const std::string& fontType = "");

	void ResetColorsToDefault();

	bool Render() override;
};

int UpdateGroupColorSet( 
	int selIdx,
	std::initializer_list<UIButton*> elements,
	const ColorSetInteractable& colsHit,
	const ColorSetInteractable& colsOther);