#pragma once
#include <cmath>
#include <wx/glcanvas.h>
#include "cvgShapes.h"

struct cvgRect
{
	float x;	// X position (leftest)
	float y;	// Y position (top)
	float w;	// Width
	float h;	// Height

	inline float EndX() const {return this->x + this->w; }	// Right value
	inline float EndY() const {return this->y + this->h; }	// Bottom value

	cvgRect();
	cvgRect(float x, float y, float w, float h);

	void Inflate(float padX_Y);
	void Inflate(float padX, float padY);

	inline float VAspect() const
	{
		if(this->w == 0.0f)
			return NAN;

		return this->h / this->w;
	}

	/// <summary>
	/// Given a vertical aspect ratio, find the largest rectangle
	/// of that aspect ratio that can fit in the invoking rectangle.
	/// </summary>
	/// <param name="vaspect">The vertical aspect to find.</param>
	/// <returns>The rectangle, or a zero retangle if none is found.</returns>
	cvgRect InnerAspect(float vaspect) const;

	static const cvgRect & Zero();
	static cvgRect MakeWidthAspect(float width, float vaspect, float x = 0.0f, float y = 0.0f);
	static cvgRect MakeHeightAspect(float height, float vaspect, float x = 0.0f, float y = 0.0f);

	inline void GLVerts_Textured()
	{
		cvgShapes::DrawBoxTex(*this);
		
	}

	inline void GLVerts()
	{
		cvgShapes::DrawBox(*this);
	}
};