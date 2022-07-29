#pragma once

#include "UIGraphic.h"
#include <functional>

/// <summary>
/// UI implementation of a slider.
/// </summary>
class UIHSlider : public UIGraphic
{
public:
	/// <summary>
	/// Value IDs that can be used for this subclass'
	/// implementation of GetValue().
	/// </summary>
	enum VID
	{
		/// <summary>
		/// The value of the slider.
		/// </summary>
		Value,

		/// <summary>
		/// The percentage of the slider value between the
		/// min and max value.
		/// </summary>
		Percent
	};

protected:

	/// <summary>
	/// The height of the crevice - i.e., the cut in the surface the
	/// slider slides along as a guide.
	/// </summary>
	const float creviceHeight = 10.0f;

	/// <summary>
	/// The maximum thumb width. The thumb width, by default, will be 
	/// the same size as the UI widget's rect's height to ensure it's 
	/// circular, but at a certain pixel limit it's clamped to avoid 
	/// the slider thumb being awkwardly wide.
	/// </summary>
	// NOTE: Consider renaming to maxThumbWidthPx.
	const float maxBarWidthPx = 50.0f;

	/// <summary>
	/// The minimum value of the slider.
	/// </summary>
	float minVal;

	/// <summary>
	/// The maximum value of the slider.
	/// </summary>
	float maxVal;

	/// <summary>
	/// The current value of the slider. This should be in
	/// the bounds of [minVal, maxVal].
	/// </summary>
	float curVal;

	/// <summary>
	/// Only used during dragging. Used to make sure the exact spot on
	/// the thumb the user clicked is respected.
	/// </summary>
	UIVec2 sliderOffs;

	/// <summary>
	/// The crevice region of the slider.
	/// </summary>
	UIRect rectInterior;

	/// <summary>
	/// The bounds of the thumb.
	/// </summary>
	UIRect rectThumb;

public:
	/// <summary>
	/// Custom function that can be assigned, called whenever 
	/// the slider value changes.
	/// </summary>
	std::function<void(float)> onSlide;

	/// <summary>
	/// The granularity to scroll when using whiffs or anything
	/// that scrolls the slider in a discretized way.
	/// </summary>
	int discreteTicks = 10;

protected:

	/// <summary>
	/// Calculate the size of the thumb width. It's based off the height
	/// of the widget, but also clamped to maxBarWidthPx.
	/// </summary>
	float ThumbWidth() const;

public:

	UIHSlider(UIBase* parent, int idx, float min, float max, float cur, const UIRect& r, TexObj::SPtr img);

	void HandleMouseDrag(const UIVec2& pos, int button) override;
	void HandleMouseDown(const UIVec2& pos, int button) override;
	void HandleMouseUp(const UIVec2& pos, int button) override;

	bool Render() override;
	float GetValue(int vid) override;

	void MoveQuantizedAmt(int quantSlices, int movedChunks);

	/// <summary>
	/// Set the value of curVal.
	/// 
	/// Even when inside of this class, prefer calling this function over
	/// modifying curVal directly, as the function manages other state
	/// management functionality.
	/// </summary>
	/// <param name="value">The new value.</param>
	void SetCurValue(float value);

	/// <summary>
	/// Recalculate cached rectangles.
	/// </summary>
	void _RecacheDirtyDimensioning();
};