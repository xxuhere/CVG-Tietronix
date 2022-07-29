#include "cvgShapes.h"
#include "cvgRect.h"

namespace cvgShapes
{ 
	const float PI = 3.14159f;

	void DrawBoxRoundedRight(const cvgRect& cvgr, float rad, int divs)
	{
		const float endX		= cvgr.EndX();
		const float endY		= cvgr.EndY();
		//
		glVertex2f(cvgr.x, cvgr.y);
		for(int i = 0; i < divs; ++i)
		{
			float lam = (0.5f * PI) - (0.5f * PI * ((float)i / (float)(divs - 1)));
			float rx = cos(lam) * rad;
			float ry = sin(lam) * rad;
			glVertex2f(endX - rad + rx, cvgr.y + rad - ry);
		}

		for(int i = 0; i < divs; ++i)
		{
			float lam = -(0.5f * PI * ((float)i / (float)(divs - 1)));
			float rx = cos(lam) * rad;
			float ry = sin(lam) * rad;
			glVertex2f(endX - rad + rx, endY - rad - ry);
		}
		glVertex2f(cvgr.x, endY);
	}

	void DrawBoxRoundedLeft(const cvgRect& cvgr, float rad, int divs)
	{
		const float endX		= cvgr.EndX();
		const float endY		= cvgr.EndY();
		//
		glVertex2f(endX, cvgr.y);
		for(int i = 0; i < divs; ++i)
		{
			float lam = (0.5f * PI) - (0.5f * PI * ((float)i / (float)(divs - 1)));
			float rx = cos(lam) * rad;
			float ry = sin(lam) * rad;
			glVertex2f(cvgr.x + rad - rx, cvgr.y + rad - ry);
		}

		for(int i = 0; i < divs; ++i)
		{
			float lam = -(0.5f * PI * ((float)i / (float)(divs - 1)));
			float rx = cos(lam) * rad;
			float ry = sin(lam) * rad;
			glVertex2f(cvgr.x + rad - rx, endY - rad - ry);
		}
		glVertex2f(endX, endY);
	}

	void DrawBox(const cvgRect& cvgr)
	{
		glVertex2i(cvgr.x,			cvgr.y);
		glVertex2i(cvgr.EndX(),		cvgr.y);
		glVertex2i(cvgr.EndX(),		cvgr.EndY());
		glVertex2i(cvgr.x,			cvgr.EndY());
	}

	void DrawBoxTex(const cvgRect& cvgr)
	{
		glTexCoord2f(	0.0f,			0.0f);
		glVertex2i(		cvgr.x,			cvgr.y);
		//
		glTexCoord2f(	1.0f,			0.0f);
		glVertex2i(		cvgr.EndX(),	cvgr.y);
		//
		glTexCoord2f(	1.0f,			1.0f);
		glVertex2i(		cvgr.EndX(),	cvgr.EndY());
		//
		glTexCoord2f(	0.0f,			1.0f);
		glVertex2i(		cvgr.x,			cvgr.EndY());
	}
}