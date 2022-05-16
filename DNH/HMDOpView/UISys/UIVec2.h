#pragma once

class UIVec2
{
public:
	float x;
	float y;

public:
	UIVec2();
	UIVec2(float x, float y);
	void Set(float x, float y);

	UIVec2 operator + (const UIVec2& v) const;
	UIVec2 operator - (const UIVec2& v) const;
	UIVec2& operator += (const UIVec2& v);
	UIVec2& operator -= (const UIVec2& v);

	UIVec2 operator * (float s) const;
	UIVec2& operator *= (float s);
	UIVec2 operator / (float s) const;
	UIVec2& operator /= (float s);

	bool operator == (const UIVec2& v2) const;
	bool operator != (const UIVec2& v2) const;

};