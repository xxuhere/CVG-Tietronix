#pragma once

#include "UIVec2.h"
#include <vector>

/// <summary>
/// Represents a 2D axis aligned rectangle.
/// </summary>
class UIRect
{
public:
	/// <summary>
	/// The top left position of the rectangle.
	/// </summary>
	UIVec2 pos;

	/// <summary>
	/// The dimensions of the rectangle.
	/// </summary>
	UIVec2 dim;

public:
	UIRect();
	UIRect(float px, float py, float w, float h);
	UIRect(const UIVec2& pos, const UIVec2& dim);

	void Set(float px, float py, float w, float h);
	void Set(const UIVec2& pos, const UIVec2& dim);
	void SetAll(float v);

	inline void SetZero(){ this->SetAll(0.0f); }

	// Adding convenient functions Right() and Bottom(),
	// and for completness/pairity sake, might as well
	// add Left() and Top(), even if not as useful.
	inline float Right() const { return this->pos.x + this->dim.x; }
	inline float Bottom() const { return this->pos.y + this->dim.y; }
	inline float Left() const { return this->pos.x; } 
	inline float Top() const { return this->pos.y; }

	inline float MidX() const { return this->pos.x + this->dim.x / 2.0f; }
	inline float MidY() const { return this->pos.y + this->dim.y / 2.0f; }
	inline UIVec2 Center() const { return UIVec2(this->MidX(), this->MidY()); }

	inline float LerpHoriz(float t) const { return this->pos.x + this->dim.x * t;}
	inline float LerpVert(float t) const { return this->pos.y + this->dim.y * t;}
	inline float LerpWidth(float t) const { return this->dim.x * t; }
	inline float LerpHeight(float t) const {return this->dim.y * t; }

	/// <summary>
	/// Expand the dimensions of the rectangle along all sides.
	/// 
	/// This version of dialate has different expansion rates for
	/// the horizontal and vertical axis.
	/// 
	/// Note that because this will expand on the top and left side,
	/// this will modify the UIRect's position.
	/// </summary>
	/// <param name="x">The horizontal expansion amount.</param>
	/// <param name="y">The vertical expansion amount.</param>
	/// <returns>The dilated rectangle.</returns>
	UIRect Dilate(float x, float y);

	/// <summary>
	/// Expand the dimensions of the rectangle along all sides.
	/// 
	/// Note that because this will expand on the top and left side,
	/// this will modify the UIRect's position.
	/// </summary>
	/// <param name="v">The amount to dilate on all 4 sides.</param>
	/// <returns>The dilated rectangle.</returns>
	UIRect Dilate(float v);

	// Set and return self functions.
	// This allows for continuous chaining in some region of code
	// where we'd rather not split out assignment of the UIRect
	// across multiple lines of code.
	inline UIRect& SS_Width(float w)	{ this->dim.x = w; return *this; }
	inline UIRect& SS_Height(float h)	{ this->dim.y = h; return *this; }
	inline UIRect& SS_X(float x)		{ this->pos.x = x; return *this; }
	inline UIRect& SS_Y(float y)		{ this->pos.y = y; return *this; }

	/// <summary>
	/// Draw the object as an untextured quad using OpenGL and GL_QUADS.
	/// </summary>
	void GLQuad() const;

	/// <summary>
	/// Function name is a misnomer, does not use OpenGL - 
	/// this function is somewhat the equivalent of GLQuad(), but 
	/// it saves out the vectors to an output parameter instead
	/// of sending them to OpenGL.
	/// </summary>
	/// <param name="outPts">The output vector.</param>
	void GLQuad(std::vector<UIVec2>& outPts) const;

	/// <summary>
	/// Draw the object as an untextured wireframe rectangle.
	/// </summary>
	void GLLineLoop() const;

	/// <summary>
	/// Draw the object as a textured quad using OpenGL And GL_QUADS.
	/// </summary>
	void GLQuadTex() const;

	/// <summary>
	/// Function is a misnomer, does not use OpenGL -
	/// this function is somewhat the equivalent of GLQuadTex(), but 
	/// it saves out the vectors to an output parameter instead
	/// of sending them to OpenGL.
	/// </summary>
	/// <param name="outUVs">The output vector for texture mapping coords.</param>
	/// <param name="outPts">The output vector for position verts.</param>
	void GLQuadTex(std::vector<UIVec2>& outUVs, std::vector<UIVec2>& outPts) const;

	/// <summary>
	/// Checks if the rectangle contains a specified point.
	/// </summary>
	/// <param name="v">The point to check against.</param>
	/// <returns>True if the rectangle contains the point; else, false.</returns>
	bool Contains(const UIVec2& v) const;

	bool operator== (const UIRect& o) const;
	bool operator!= (const UIRect& o) const;
};