#include "UIVBulkSlider.h"
#include <wx/glcanvas.h>

UIVBulkSlider::UIVBulkSlider(UIBase* parent, int idx, float min, float max, float cur, const UIRect& r)
	: UIBase(parent, idx, r)
{
	this->minVal = minVal;
	this->maxVal = maxVal;
	this->curVal = curVal;

	this->fontMeter = FontMgr::GetInstance().GetFont(10);
}

void UIVBulkSlider::HandleMouseDrag(const UIVec2& pos, int button)
{
	this->UIBase::HandleMouseDrag(pos, button);
}

void UIVBulkSlider::HandleMouseDown(const UIVec2& pos, int button)
{
	this->UIBase::HandleMouseDown(pos, button);
}

void UIVBulkSlider::HandleMouseUp(const UIVec2& pos, int button)
{
	this->UIBase::HandleMouseUp(pos, button);
}

bool UIVBulkSlider::Render()
{
	if(!this->selfVisible)
		return false;

	// The left half is the meter,
	// The right half is spacing for the meter.
	//
	// If a setup is needed where the bar is centered
	// in the middle, the layout must take that into
	// consideration and add blank space to manually
	// center it.

	UIVec2 tl = this->rect.pos;
	UIVec2 bl(tl.x, tl.y + this->rect.dim.y);
	UIVec2 tr(tl.x + this->rect.dim.x * 0.5, tl.y);
	UIVec2 br(tr.x, bl.y);

	glDisable(GL_TEXTURE_2D);

	// Backwash
	glColor3f(0.25f, 0.25f, 0.25f);
	glBegin(GL_QUADS);
		glVertex2f(tl.x, tl.y);
		glVertex2f(tr.x, tr.y);
		glVertex2f(br.x, br.y);
		glVertex2f(bl.x, bl.y);
	glEnd();

	// U-shaped outline
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINE_STRIP);
		glVertex2f(tl.x, tl.y);
		glVertex2f(bl.x, bl.y);
		glVertex2f(br.x, br.y);
		glVertex2f(tr.x, tr.y);
	glEnd();

	const float hPad = 10.0f;
	UIVec2 tli(tl.x + hPad, tl.y);
	UIVec2 bli(bl.x + hPad, bl.y);
	UIVec2 tri(tr.x - hPad, tr.y);
	UIVec2 bri(br.x - hPad, br.y);
	glBegin(GL_LINE);
		glVertex2fv((float*)&tli);
		glVertex2fv((float*)&tri);
		glVertex2fv((float*)&bri);
		glVertex2fv((float*)&bli);
	glEnd();

	float usableSpace = this->rect.dim.y - this->barHeightPx;
	float percent = this->curVal / (this->maxVal - this->minVal);
	float bottomPx = percent * usableSpace;
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
		glVertex2f(tli.x, tli.y);
		glVertex2f(tli.x, tli.y - bottomPx);
		glVertex2f(tri.x, tli.y - bottomPx);
		glVertex2f(tri.x, tli.y);
	glEnd();

	UIColor4& hotCol = this->uiCols.GetContexedColor(this->pressedCt, this->isHovered);
	hotCol.GLColor4();
	glBegin(GL_QUADS);
		glVertex2f(tli.x, tli.y - bottomPx);
		glVertex2f(tli.x, tli.y - bottomPx - this->barHeightPx);
		glVertex2f(tri.x, tli.y - bottomPx - this->barHeightPx);
		glVertex2f(tri.x, tli.y - bottomPx);
	glEnd();

	return true;
}