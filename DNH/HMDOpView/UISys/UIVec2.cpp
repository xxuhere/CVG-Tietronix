#include "UIVec2.h"
#include "UIRect.h"

UIVec2::UIVec2()
{}

UIVec2::UIVec2(float x, float y)
{
	this->x = x;
	this->y = y;
}

void UIVec2::Set(float x, float y)
{
	this->x = x;
	this->y = y;
}

UIVec2 UIVec2::operator + (const UIVec2& v) const
{
	return UIVec2(
		this->x + v.x, 
		this->y + v.y);
}

UIVec2 UIVec2::operator - (const UIVec2& v) const
{
	return UIVec2(
		this->x - v.x, 
		this->y - v.y);
}

UIVec2& UIVec2::operator += (const UIVec2& v)
{
	this->x += v.x;
	this->y += v.y;
	return *this;
}

UIVec2& UIVec2::operator -= (const UIVec2& v)
{
	this->x -= v.x;
	this->y -= v.y;
	return *this;
}

UIVec2 UIVec2::operator * (float s) const
{
	return UIVec2(
		this->x * s, 
		this->y * s);
}

UIVec2& UIVec2::operator *= (float s)
{
	this->x *= s;
	this->y *= s;
	return *this;
}

UIVec2 UIVec2::operator / (float s) const
{
	return UIVec2(
		this->x / s,
		this->y / s);
}

UIVec2& UIVec2::operator /= (float s)
{
	this->x /= s;
	this->y /= s;
	return *this;
}

UIVec2 UIVec2::operator * (const UIVec2& o) const
{
	return UIVec2(
		this->x * o.x,
		this->y * o.y);
}

UIVec2& UIVec2::operator *= (const UIVec2& o)
{
	this->x *= o.x;
	this->y *= o.y;
	return *this;
}

UIVec2 UIVec2::operator / (const UIVec2& o) const
{
	return UIVec2(
		this->x / o.x,
		this->y / o.y);
}

UIVec2& UIVec2::operator /= (const UIVec2& o)
{
	this->x /= o.x;
	this->y /= o.y;
	return *this;
}

bool UIVec2::operator == (const UIVec2& v2) const
{
	return 
		this->x == v2.x && 
		this->y == v2.y;
}

bool UIVec2::operator != (const UIVec2& v2) const
{
	return
		this->x != v2.x ||
		this->y != v2.y;
}

bool UIVec2::InRectBounds(const UIVec2& pt, const UIVec2& topLeft, const UIVec2& dim)
{
	return 
		pt.x >= topLeft.x && 
		pt.y >= topLeft.y &&
		pt.x <= topLeft.x + dim.x &&
		pt.y <= topLeft.y + dim.y;
}

bool UIVec2::InRectBounds(const UIVec2& pt, const UIRect& r)
{
	return InRectBounds(pt, r.pos, r.dim);
}

void UIVec2::SetOne()
{
	this->x = 1.0f;
	this->y = 1.0f;
}

void UIVec2::SetZero()
{
	this->x = 0.0f;
	this->y = 0.0f;
}