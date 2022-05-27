#pragma once
#include "UIBase.h"
#include "../FontMgr.h"
#include <functional>

/// <summary>
/// Eccentric, graduated cylinder style, vertical slider.
/// 
/// Follows CVG HMDOpView design document but may be deprecated/obsolete.
/// </summary>
class UIVBulkSlider : public UIBase
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
	// The horizontal padding from the slider to 
	// the horizontal ends of the slider.
	const float hPad = 5.0f;

	/// <summary>
	/// The height of the thumbs.
	/// </summary>
	const float barHeightPx = 40.0f;

	/// <summary>
	/// The minimum value of the slider.
	/// </summary>
	float minVal;

	/// <summary>
	/// The maximum value of the slider. This should be greater
	/// than minVal.
	/// </summary>
	float maxVal;

	/// <summary>
	/// The current value of the slider. This should be in
	/// the bounds of [minVal, maxVal].
	/// </summary>
	float curVal;

	/// <summary>
	/// The font used to draw the value to the right of the thumb.
	/// </summary>
	// TODO: Consider using monotype
	FontWU fontMeter;

	/// <summary>
	/// Only used during dragging. Used to make sure the exact spot on
	/// the thumb the user clicked is respected.
	/// </summary>
	UIVec2 sliderOffs;

	/// <summary>
	/// If true, the thumb is in the middle of a click and drag. This is
	/// needed on top of the UISys dragging, because not all of the
	/// widget's region is draggable.
	/// </summary>
	bool dragged = false;

	/// <summary>
	/// The bounds of just the slider portion.
	/// </summary>
	UIRect rectWhole;

	/// <summary>
	/// The entire interior (horizontally padded) region
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

public:

	UIVBulkSlider(UIBase* parent, int idx, float min, float max, float cur, const UIRect& r);

	void HandleMouseDrag(const UIVec2& pos, int button) override;
	void HandleMouseDown(const UIVec2& pos, int button) override;
	void HandleMouseUp(const UIVec2& pos, int button) override;

	bool Render() override;

	void SetCurValue(float value);
	float GetValue(int vid) override;

	// Called whenever the drawing and boundaries for the slider
	// need to be updated. This is both for when its size has changed,
	// or its value (and sub-items) have been modified.
	void _RecacheDirtyDimensioning();
};