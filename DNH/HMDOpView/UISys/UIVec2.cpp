#include "UIVec2.h"

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