#pragma once

struct cvgRect;

namespace cvgShapes
{
	void DrawBoxRoundedRight(const cvgRect& cvgr, float rad, int divs = 12);
	void DrawBoxRoundedLeft(const cvgRect& cvgr, float rad, int divs = 12);
	void DrawBox(const cvgRect& cvgr);
	void DrawBoxTex(const cvgRect& cvgr);
}