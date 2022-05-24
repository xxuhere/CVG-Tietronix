#pragma once

class UIRect;

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

	// Scalar multiplication and division.
	UIVec2 operator * (float s) const;
	UIVec2& operator *= (float s);
	UIVec2 operator / (float s) const;
	UIVec2& operator /= (float s);

	// Component-wise multiplication and division
	UIVec2 operator * (const UIVec2& o) const;
	UIVec2& operator *= (const UIVec2& o);
	UIVec2 operator / (const UIVec2& o) const;
	UIVec2& operator /= (const UIVec2& o);

	bool operator == (const UIVec2& v2) const;
	bool operator != (const UIVec2& v2) const;

public:
	static bool InRectBounds(
		const UIVec2& pt, 
		const UIVec2& topLeft, 
		const UIVec2& dim);

	static bool InRectBounds(
		const UIVec2& pt,
		const UIRect& r);

	void SetOne();
	void SetZero();

};