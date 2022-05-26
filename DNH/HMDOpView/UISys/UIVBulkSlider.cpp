#include "UIVBulkSlider.h"
#include <wx/glcanvas.h>
#include <sstream>
#include <iomanip>

UIVBulkSlider::UIVBulkSlider(UIBase* parent, int idx, float min, float max, float cur, const UIRect& r)
	: UIBase(parent, idx, r)
{
	this->minVal = min;
	this->maxVal = max;
	this->curVal = cur;

	this->fontMeter = FontMgr::GetInstance().GetFont(10);
}

void UIVBulkSlider::HandleMouseDrag(const UIVec2& pos, int button)
{
	this->UIBase::HandleMouseDrag(pos, button);

	if(!this->dragged)
		return;

	float adjMouseY = pos.y - this->sliderOffs.y;
	float topUsable = this->rectWhole.Top();
	float botUsable = this->rectWhole.Bottom() - barHeightPx;

	// Inverse lerp
	float lambda = std::clamp((adjMouseY - topUsable)/(botUsable - topUsable), 0.0f, 1.0f);
	lambda = 1.0f - lambda;
	// Forward lerp to find value
	float newVal = this->minVal + (this->maxVal - this->minVal) * lambda;
	this->SetCurValue(newVal);
}

void UIVBulkSlider::HandleMouseDown(const UIVec2& pos, int button)
{
	this->UIBase::HandleMouseDown(pos, button);

	if(this->rectWhole.Contains(pos) == false)
		return;

	if(this->rectThumb.Contains(pos))
	{
		// If clicked directly onto the thumb
		this->sliderOffs = pos - this->rectThumb.pos;
	}
	else
	{
		// Move the mouse Y halfway down so the click is treated like 
		// the halfway point of thumb instead of the top.
		float adjMouseY = pos.y + barHeightPx * 0.5f;

		// Inverse lerp from screen and mouse position
		float topUsable = this->rectWhole.pos.y + barHeightPx;
		float botUsable = this->rectWhole.Bottom();
		float lambda = std::clamp((adjMouseY - topUsable)/(botUsable - topUsable), 0.0f, 1.0f);
		// The bottom is 0.0 for this widget, not the top
		lambda = 1.0f - lambda;
		// Forward lerp to find the correct value.
		float newVal = this->minVal + (this->maxVal - this->minVal) * lambda;
		this->SetCurValue(newVal);

		this->_RecacheDirtyDimensioning(); // Needed to calc sliderOffs
		this->sliderOffs = pos - this->rectThumb.pos;
	}

	dragged = true;

	// The contents may not be dirty because we do a recalculation if needed
	// via _RecacheDirtyDimensioning(), but just for rigor's sake.
	this->FlagContentsDirty();
}

void UIVBulkSlider::HandleMouseUp(const UIVec2& pos, int button)
{
	this->dragged = false;
	this->UIBase::HandleMouseUp(pos, button);
}

bool UIVBulkSlider::Render()
{
	if(!this->selfVisible)
		return false;

	if(this->IsContentsDirty())
	{
		this->_RecacheDirtyDimensioning();
		this->dirtyContents = false;
	}

	

	glDisable(GL_TEXTURE_2D);

	// Backwash
	glColor3f(0.25f, 0.25f, 0.25f);
	rectWhole.GLQuad();

	//		DRAW SLIDER CREVICES
	//////////////////////////////////////////////////

	const float iRight	= this->rectInterior.Right();
	const float iLeft	= this->rectInterior.Left();
	const float iTop	= this->rectInterior.Top();
	const float iTBot	= this->rectThumb.Top();
	const float iBTop	= this->rectThumb.Bottom();
	const float iBot	= this->rectInterior.Bottom();

	glBegin(GL_QUADS);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex2f(iLeft,	iTBot);
		glVertex2f(iLeft,	iTop);
		glVertex2f(iRight,	iTop);
		glVertex2f(iRight,	iTBot);

		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex2f(iLeft,	iBot);
		glVertex2f(iLeft,	iBTop);
		glVertex2f(iRight,	iBTop);
		glVertex2f(iRight,	iBot);
	glEnd();


	//		DRAW THUMB
	//////////////////////////////////////////////////

	UIColor4 colCur = 
		this->uiCols.GetContexedColor(
			this->pressedCt, 
			this->isHovered);

	colCur.GLColor4();
	this->rectThumb.GLQuad();
	glColor3f(0.0f, 0.0f, 0.0f);
	this->rectThumb.GLLineLoop();

	// SIDE TEXT
	std::stringstream sstrm;
	sstrm << std::fixed;
	sstrm << std::setprecision(3);
	sstrm << this->curVal;
	//
	const float textShiftRight = 5.0f;
	colCur.GLColor4();
	this->fontMeter.RenderFont(
		sstrm.str().c_str(), 
		this->rectThumb.Right() + textShiftRight, 
		this->rectThumb.MidY());


	if(this->IsRegisteredSelected())
	{ 
		// U-shaped outline
		glColor3f(0.0f, 0.0f, 0.0f);
		this->rectWhole.GLLineLoop();
	}
	
	return true;
}

void UIVBulkSlider::SetCurValue(float value)
{
	value = std::clamp(value, this->minVal, this->maxVal);

	if(value == this->curVal)
		return;

	this->curVal = value;
	this->SubmitValue(VID::Value, this->curVal);
	this->FlagContentsDirty();

	if(this->onSlide)
		this->onSlide(this->curVal);
}

float UIVBulkSlider::GetValue(int vid)
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

void UIVBulkSlider::_RecacheDirtyDimensioning()
{
	this->rectWhole.Set(
		UIVec2(this->rect.pos.x, this->rect.pos.y),
		UIVec2(this->rect.dim.x * 0.5, this->rect.dim.y));

	this->rectInterior.Set(
		UIVec2(this->rectWhole.pos.x + hPad, this->rectWhole.pos.y), 
		UIVec2(this->rectWhole.dim.x - hPad * 2.0f, this->rectWhole.dim.y));

	float usableSpace = this->rect.dim.y - this->barHeightPx;
	float lambda = this->curVal/(this->maxVal - this->minVal);
	float botThumb = usableSpace * lambda;

	this->rectThumb.Set(
		UIVec2(
			this->rectWhole.pos.x, 
			this->rectWhole.Bottom() - botThumb - barHeightPx),
		UIVec2(this->rectWhole.dim.x, barHeightPx));
}