#pragma once
#include "UIVec2.h"

class UIRect
{
public:
	UIVec2 pos;
	UIVec2 dim;

public:
	UIRect();
	UIRect(float px, float py, float w, float h);
	UIRect(const UIVec2& pos, const UIVec2& dim);

	void GLQuad();
	void GLQuadTex();
};