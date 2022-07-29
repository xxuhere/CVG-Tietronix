#pragma once

struct cvgRect;


// Procedural shapes library.
// 
// Deprecated, as this was to draw assets for major drawing
// systems were available.
// (wleu 05/26/2022)

namespace cvgShapes
{
	void DrawBoxRoundedRight(const cvgRect& cvgr, float rad, int divs = 12);
	void DrawBoxRoundedLeft(const cvgRect& cvgr, float rad, int divs = 12);
	void DrawBox(const cvgRect& cvgr);
	void DrawBoxTex(const cvgRect& cvgr);
}