#include "UIHSlider.h"
#include <algorithm>
#include <wx/glcanvas.h>
#include <algorithm>

UIHSlider::UIHSlider(UIBase* parent, int idx, float min, float max, float cur, const UIRect& r, TexObj::SPtr img)
	: UIGraphic(parent, idx, r, img)
{
	this->minVal = min;
	this->maxVal = max;
	this->curVal = cur;
}

void UIHSlider::HandleMouseDrag(const UIVec2& pos, int button)
{
	float barWidth = this->ThumbWidth();

	float adjMouseX = pos.x - this->sliderOffs.x;
	float leftUsable = this->rect.Left();
	float rightUsable = this->rect.Right() - barWidth;

	// Inverse lerp
	float lambda = std::clamp((adjMouseX - leftUsable)/(rightUsable - leftUsable), 0.0f, 1.0f);
	// Forward lerp to find value
	float newVal = this->minVal + (this->maxVal - this->minVal) * lambda;
	this->SetCurValue(newVal);
}

void UIHSlider::HandleMouseDown(const UIVec2& pos, int button)
{
	this->UIGraphic::HandleMouseDown(pos, button);

	if(this->rectThumb.Contains(pos))
	{
		// If clicked directly onto the thumb
		this->sliderOffs = pos - this->rectThumb.pos;
	}
	else
	{
		float thumbWidth = this->ThumbWidth();

		float adjMouseX = pos.x - thumbWidth * 0.5f;

		// Inverse lerp from screen and mouse position
		float leftUsable = this->rect.pos.x;
		float rightUsable = this->rect.Right() - thumbWidth;
		float lambda = std::clamp((adjMouseX - leftUsable)/(rightUsable - leftUsable), 0.0f, 1.0f);
		// Forward lerp to find the correct value.
		float newVal = this->minVal + (this->maxVal - this->minVal) * lambda;
		this->SetCurValue(newVal);

		this->_RecacheDirtyDimensioning(); // Needed to calc sliderOffs
		this->sliderOffs = pos - this->rectThumb.pos;
	}

	// The contents may not be dirty because we do a recalculation if needed
	// via _RecacheDirtyDimensioning(), but just for rigor's sake.
	this->FlagContentsDirty();
}

void UIHSlider::HandleMouseUp(const UIVec2& pos, int button)
{
	this->UIGraphic::HandleMouseUp(pos, button);
}

bool UIHSlider::Render()
{
	if(!this->selfVisible)
		return false;

	if(this->IsContentsDirty())
	{
		this->_RecacheDirtyDimensioning();
		this->dirtyContents = false;
	}

	glColor3f(0.0f, 0.0f, 0.0f);
	this->rectInterior.GLQuad();

	this->_DrawVerts();

	return true;
}

void UIHSlider::SetCurValue(float value)
{
	value = std::clamp(value, this->minVal, this->maxVal);

	if(value == this->curVal)
		return;

	this->curVal = value;
	this->SubmitValue(this->curVal, VID::Value);
	this->FlagContentsDirty();

	if(this->onSlide)
		this->onSlide(this->curVal);
}

float UIHSlider::GetValue(int vid)
{
	switch(vid)
	{
	case VID::Value:
		return this->curVal;
		break;

	case VID::Percent:
		// Inverse lerp
		return (this->curVal - this->minVal)/(this->maxVal - this->minVal);
		break;
	}
	return UIBase::GetValue(vid);
}

float UIHSlider::ThumbWidth() const
{
	return std::min(maxBarWidthPx, this->rect.dim.y);
}

void UIHSlider::_RecacheDirtyDimensioning()
{
	float yCenter = this->rect.MidY();
	float halfCrevH = creviceHeight * 0.5f;

	this->rectInterior.Set(
		UIVec2(this->rect.pos.x, yCenter - halfCrevH), 
		UIVec2(this->rect.dim.x, creviceHeight));

	float thumbWidth = this->ThumbWidth();

	float useXSt = this->rect.pos.x;
	float useXEn = this->rect.Right() - thumbWidth;

	float lambda = (this->curVal - this->minVal)/(this->maxVal - this->minVal);
	float thumbX = useXSt + (useXEn - useXSt) * lambda;
	this->rectThumb.Set(
		UIVec2(thumbX, this->rect.pos.y), 
		UIVec2(thumbWidth, this->rect.dim.y));

	this->_RebuildVerts(this->rectThumb);
}