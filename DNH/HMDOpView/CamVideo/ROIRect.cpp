#include "ROIRect.h"


ROIRect::ROIRect(int x, int y, int w, int h)
{
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
}

cv::Rect ROIRect::ToCVRect() const
{
	return cv::Rect(this->x, this->y, this->w, this->h);
}

ROIRect ROIRect::Intersect(const ROIRect& a, const ROIRect& b)
{
	ROIRect ret = a;

	// Make sure the top and left are not
	// outside the bounds of b
	//
	// This is done in two steps
	//
	// - First we check how the dimension would shrink if we moved it out 
	//	and apply that (if needed).
	// - Then we actually move it out.
	if(b.x > ret.x)
	{
		ret.w -= b.x - ret.x;
		ret.x = b.x;
	}

	if(b.y > ret.y)
	{
		ret.h -= b.y - ret.y;
		ret.y = b.y;
	}

	// Make sure the width and height are not 
	// outside the bounds of b.
	if(b.w < ret.w)
	{
		ret.w = b.w;
	}
	if(b.h < ret.h)
	{
		ret.h = b.h;
	}
	return ret;
}

void ROIRect::AddDim(int w, int h)
{
	this->w += w;
	this->h += h;
}

void ROIRect::AddPos(int x, int y)
{
	this->x += x;
	this->y += y;
}

void ROIRect::ShrinkSameDims_C(ROIRect& a, ROIRect& b)
{
	// Basically check if one rectangle's dimension is larger
	// than the other, if so, shrink itself down. Then do the
	// same thing with the rectangles switched in the comparison.
	if(a.w > b.w)
	{
		int half = (a.w - b.w)/2;
		a.x += half;
		a.w = b.w;
	}
	if(a.h > b.h)
	{
		int half = (a.h - b.h)/2;
		a.y += half;
		a.h = b.h;
	}
	if(b.w > a.w)
	{
		int half = (b.w - a.w)/2;
		b.x += half;
		b.w = a.w;
	}
	if(b.h > a.h)
	{
		int half = (b.h - a.h)/2;
		b.y += half;
		b.h = a.h;
	}
}