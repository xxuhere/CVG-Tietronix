#include "UIRect.h"
#include <wx/glcanvas.h>

UIRect::UIRect()
{}

UIRect::UIRect(float px, float py, float w, float h)
{
	this->pos.x = px;
	this->pos.y = py;
	this->dim.x = w;
	this->dim.y = h;
}

UIRect::UIRect(const UIVec2& pos, const UIVec2& dim)
{
	this->pos = pos;
	this->dim = dim;
}

void UIRect::Set(float px, float py, float w, float h)
{
	this->pos.x = px;
	this->pos.y = py;
	this->dim.x = w;
	this->dim.y = h;
}

void UIRect::Set(const UIVec2& pos, const UIVec2& dim)
{
	this->pos = pos;
	this->dim = dim;
}

void UIRect::SetAll(float v)
{
	this->pos.x = v;
	this->pos.y = v;
	this->dim.x = v;
	this->dim.y = v;
}

UIRect UIRect::Dilate(float x, float y)
{
	return UIRect(
		this->pos.x - x,
		this->pos.y - y,
		this->dim.x + x * 2.0f,
		this->dim.y + y * 2.0f);
}

UIRect UIRect::Dilate(float v)
{
	return UIRect(
		this->pos.x - v,
		this->pos.y - v,
		this->dim.x + v * 2.0f,
		this->dim.y + v * 2.0f);
}

UIRect UIRect::DilateAtOrigin(float x, float y)
{
	return UIRect(
		-x,
		-y,
		this->dim.x + x * 2.0f,
		this->dim.y + y * 2.0f);
}

UIRect UIRect::DilateAtOrigin(float v)
{
	return UIRect(
		-v,
		-v,
		this->dim.x + v * 2.0f,
		this->dim.y + v * 2.0f);
}

void UIRect::GLQuad()
{
	float right = this->pos.x + this->dim.x;
	float bottom = this->pos.y + this->dim.y;

	glBegin(GL_QUADS);
		glVertex2f(this->pos.x,		this->pos.y);
		glVertex2f(right,			this->pos.y);
		glVertex2f(right,			bottom );
		glVertex2f(this->pos.x,		bottom );
	glEnd();
}

void UIRect::GLQuad(std::vector<UIVec2>& outPts)
{
	float right = this->pos.x + this->dim.x;
	float bottom = this->pos.y + this->dim.y;

	outPts.push_back(UIVec2(this->pos.x,	this->pos.y));
	outPts.push_back(UIVec2(right,			this->pos.y));
	outPts.push_back(UIVec2(right,			bottom));
	outPts.push_back(UIVec2(this->pos.x,	bottom));
}

void UIRect::GLQuadTex()
{
	float right = this->pos.x + this->dim.x;
	float bottom = this->pos.y + this->dim.y;

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(this->pos.x,		this->pos.y);
		//
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(right,			this->pos.y);
		//
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(right,			bottom);
		//
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(this->pos.x,		bottom);
	glEnd();
}

void UIRect::GLQuadTex(
	std::vector<UIVec2>& outUVs, 
	std::vector<UIVec2>& outPts)
{
	float right = this->pos.x + this->dim.x;
	float bottom = this->pos.y + this->dim.y;

	outUVs.push_back(UIVec2(0.0f, 0.0f));
	outPts.push_back(UIVec2(this->pos.x,	this->pos.y));
	//
	outUVs.push_back(UIVec2(1.0f, 0.0f));
	outPts.push_back(UIVec2(right,			this->pos.y));
	//
	outUVs.push_back(UIVec2(1.0f, 1.0f));
	outPts.push_back(UIVec2(right,			bottom));
	//
	outUVs.push_back(UIVec2(0.0f, 1.0f));
	outPts.push_back(UIVec2(this->pos.x,	bottom));
}

bool UIRect::operator== (const UIRect& o) const
{
	// Arguably we should leverage UIVec2::operator==, but (admittedly) 
	// I'm trying micro-optimize a little and hopefully avoid incurring
	// function call overhead by duplicating the vector == comparisons 
	// inline.

	return 
		this->pos.x == o.pos.x && 
		this->pos.y == o.pos.y && 
		this->dim.x == o.dim.x && 
		this->dim.y == o.dim.y;
}

bool UIRect::operator!= (const UIRect& o) const
{
	return 
		this->pos.x != o.pos.x || 
		this->pos.y != o.pos.y || 
		this->dim.x != o.dim.x || 
		this->dim.y != o.dim.y;
}