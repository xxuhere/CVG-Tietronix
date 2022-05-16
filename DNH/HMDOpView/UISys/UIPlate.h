#pragma once
#include "UIBase.h"
#include "UIColor4.h"

class UIPlate : public UIBase
{
public:
	enum Mode
	{
		Fill,
		Slice
	};

public:
	float ULeft = 0.3f;
	float URight = 0.7f;
	float VTop = 0.3f;
	float VBot = 0.7f;

	float pxLeft = 10;
	float pxRight = 10;
	float pxTop = 10;
	float pxBot = 10;

	UIColor4 colNormal;
	UIColor4 colHover;

	UIVec2 lastDim;

public:
	UIPlate(int idx, const UIRect& r);

	void Render() override;
};