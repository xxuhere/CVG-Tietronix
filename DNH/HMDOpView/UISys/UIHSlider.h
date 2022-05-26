#pragma once
#include "UIGraphic.h"
#include <functional>

class UIHSlider : public UIGraphic
{
public:
	enum VID
	{
		Value,
		Percent
	};

protected:
	const float vPad = 5.0f;
	const float creviceHeight = 10.0f;
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

protected:
	float ThumbWidth() const;

public:

	UIHSlider(UIBase* parent, int idx, float min, float max, float cur, const UIRect& r, TexObj::SPtr img);

	void HandleMouseDrag(const UIVec2& pos, int button) override;
	void HandleMouseDown(const UIVec2& pos, int button) override;
	void HandleMouseUp(const UIVec2& pos, int button) override;

	bool Render() override;

	void SetCurValue(float value);
	float GetValue(int vid) override;

	void _RecacheDirtyDimensioning();
};