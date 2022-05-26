#pragma once
#include "UIBase.h"
#include <string>
#include "UIGraphic.h"
#include <initializer_list>
#include <vector>

class UIButton : public UIGraphic
{
public:
	std::string text;
	FontWU font;

	UIColor4 textColor;

	

private:
	void _InitializeColors();

public:
	std::function<void(int)> onClick;

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

	void HandleClick(int button) override;

	bool Render() override;
};

int UpdateGroupColorSet( 
	int selIdx,
	std::initializer_list<UIButton*> elements,
	const ColorSetInteractable& colsHit,
	const ColorSetInteractable& colsOther);