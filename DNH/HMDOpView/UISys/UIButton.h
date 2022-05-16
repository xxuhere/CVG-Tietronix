#pragma once
#include "UIBase.h"
#include <string>
#include "../TexObj.h"
class UIButton : public UIBase
{
public:
	TexObj::SPtr image;

public:
	UIButton(int idx, const UIRect& r, const std::string& filePath);
	UIButton(int idx, const UIRect& r, TexObj::SPtr img);

	void Render() override;
};