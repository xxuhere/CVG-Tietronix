#pragma once
#include "UIVec2.h"

class UIRect
{
public:
	UIVec2 pos;
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

	UIRect Dilate(float x, float y);
	UIRect Dilate(float v);
	UIRect DilateAtOrigin(float x, float y);
	UIRect DilateAtOrigin(float v);

	// Set and return self functions.
	inline UIRect& SS_Width(float w)	{ this->dim.x = w; return *this; }
	inline UIRect& SS_Height(float h)	{ this->dim.y = h; return *this; }
	inline UIRect& SS_X(float x)		{ this->pos.x = x; return *this; }
	inline UIRect& SS_Y(float y)		{ this->pos.y = y; return *this; }

	void GLQuad();
	void GLQuadTex();

	bool operator== (const UIRect& o) const;
	bool operator!= (const UIRect& o) const;
};