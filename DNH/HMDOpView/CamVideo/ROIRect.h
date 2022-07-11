#pragma once
#include <opencv2/core/types.hpp>

/// <summary>
/// Utility class to represent integer ROI rectangles
/// for the needs of the CVG project.
/// 
/// This is mainly to supplement cv::Rect functionality
/// in a representation that the project owns.
/// 
/// These rectangles are in the coordinate convention of
/// cv::Rect.
/// </summary>
class ROIRect
{
public:

	/// <summary>
	/// The x position for the left of the rectangle.
	/// </summary>
	int x;

	/// <summary>
	/// The y position for the top of the rectangle.
	/// </summary>
	int y;

	/// <summary>
	/// The width of the rectangle.
	/// </summary>
	int w;

	/// <summary>
	/// The height of the rectangle.
	/// </summary>
	int h;

public:

	ROIRect(int x, int y, int w, int h);

	/// <summary>
	/// Convert the ROIRect to a cv::Rect.
	/// </summary>
	/// <returns>The OpenCV rectangle.</returns>
	cv::Rect ToCVRect() const;

	/// <summary>
	/// Add to the dimension components.
	/// </summary>
	/// <param name="w">The amount to add to the width.</param>
	/// <param name="h">The amount to add to the height.</param>
	void AddDim(int w, int h);

	/// <summary>
	/// Add to the position components.
	/// </summary>
	/// <param name="x">The amount to add to the x.</param>
	/// <param name="y">The amount to add to the y.</param>
	void AddPos(int x, int y);

	inline int Right() const 
	{ return this->x + this->w; }

	inline int Bottom() const
	{ return this->y + this->h; }

public:
	/// <summary>
	/// Given two rectangles, return their intersecting region.
	/// </summary>
	/// <param name="a">The first rectangle.</param>
	/// <param name="b">The second rectangle.</param>
	/// <returns>The intersected rectangle.</returns>
	static ROIRect Intersect(const ROIRect& a, const ROIRect& b);

	/// <summary>
	/// Shrink two rectangles to have the same dimensions, this will
	/// contract the rectangles from their respective centers.
	/// </summary>
	/// <param name="a">Rectangle A</param>
	/// <param name="b">Rectcangle B</param>
	static void ShrinkSameDims_C(ROIRect& a, ROIRect& b);
};