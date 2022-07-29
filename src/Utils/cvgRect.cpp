#include "cvgRect.h"

const cvgRect & cvgRect::Zero()
{
	static cvgRect z(0.0f, 0.0f, 0.0f, 0.0f);
	return z;
}

cvgRect::cvgRect()
{}

cvgRect::cvgRect(float x, float y, float w, float h)
{
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
}

void cvgRect::Inflate(float padX_Y)
{
	this->x -= padX_Y;
	this->w += padX_Y * 2.0f;
	this->y -= padX_Y;
	this->h += padX_Y * 2.0f;
}

void cvgRect::Inflate(float padX, float padY)
{
	this->x -= padX;
	this->w += padX * 2.0f;
	this->y -= padY;
	this->h += padY * 2.0f;
}

cvgRect cvgRect::InnerAspect(float vaspect) const
{
	float vaThis = this->VAspect();

	// early error detections
	if(std::isnan(vaThis))
		return cvgRect(0.0f, 0.0f, 0.0f, 0.0f);

	if(vaThis == vaspect)
		return *this;

	float cx = this->x + this->w * 0.5f;
	float cy = this->y + this->h * 0.5f;
	if(vaspect > vaThis)
	{
		// If the target is taller, the height needs to
		// match, and then shrink in the width.
		float newH = this->h;
		float newW = this->w / vaThis;
		return cvgRect(cx - newW * 0.5f, cy - newH * 0.5f, newW, newH);
	}
	else
	{ 
		float newW = this->w;
		float newH = this->h * vaThis;
		return cvgRect(cx - newW * 0.5f, cy - newH * 0.5f, newW, newH);
	}
}

cvgRect cvgRect::MakeWidthAspect(float width, float vaspect, float x, float y)
{
	return cvgRect(x, y, width, width * vaspect);
}

cvgRect cvgRect::MakeHeightAspect(float height, float vaspect, float x, float y)
{
	return cvgRect(x, y, height / vaspect, vaspect);
}