#pragma once

class UIRect;

// NOTE: The codebase is currently at c++2017. As of writing this comment,
// std::lerp is too new to use because it's a C++2020 function.
inline float Lerp(float a, float b, float t)
{
	return a + (b - a) * t;
}

/// <summary>
/// 2D vector.
/// </summary>
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
	/// <summary>
	/// 
	/// </summary>
	/// <param name="pt"></param>
	/// <param name="topLeft"></param>
	/// <param name="dim"></param>
	/// <returns></returns>
	static bool InRectBounds(
		const UIVec2& pt, 
		const UIVec2& topLeft, 
		const UIVec2& dim);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="pt"></param>
	/// <param name="r"></param>
	/// <returns></returns>
	static bool InRectBounds(
		const UIVec2& pt,
		const UIRect& r);

	/// <summary>
	/// Set both the x and y component to 1.0f.
	/// </summary>
	void SetOne();

	/// <summary>
	/// Set both the x and y component to 0.0f.
	/// </summary>
	void SetZero();

	inline static UIVec2 Lerp(const UIVec2& a, const UIVec2& b, float t)
	{
		return UIVec2(
			::Lerp(a.x, b.x, t),
			::Lerp(a.y, b.y, t));
	}

};