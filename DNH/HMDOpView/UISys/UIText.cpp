#include "UIText.h"
#include "../Utils/cvgAssert.h"

UIText::UIText(UIBase* parent, int idx, const std::string& text, int size, const UIRect& r)
	: UIBase(parent, idx, r)
{
	this->fontHandle = FontMgr::GetInstance().GetFont(size);
	this->SetText(text);
}

void UIText::SetText(const std::string& text)
{
	this->text = text;
	if(this->fontHandle.IsValid())
		this->cachedHAdvance = this->fontHandle.GetAdvance(this->text.c_str());
	else
		this->cachedHAdvance = -1;
}

bool UIText::Render()
{
	if(!this->selfVisible)
		return false;

	if(!this->fontHandle.IsValid())
		return false;

	// We're not going to assume there's every any children
	// to font widgets.

	// Top-left of where to draw;
	float xPos;
	float yPos;

	switch(this->halign)
	{
	case HTextAlign::Left:
		xPos =  this->rect.pos.x;
		break;

	case HTextAlign::Middle:
		xPos = this->rect.pos.x + (this->rect.dim.x - this->cachedHAdvance)/2.0f;
		break;

	case HTextAlign::Right:
		xPos = this->rect.Right() - this->cachedHAdvance;
		break;

	default:
		xPos = 0.0f;
		cvgAssert(false, "Unhandled HAlign calculation for UIText render.");
		break;
	}

	// Right now multilines aren't handled.
	switch(this->valign)
	{
	case VTextAlign::Top:
		yPos = this->rect.pos.y;
		break;

	case VTextAlign::Center:
		yPos = this->rect.pos.y + (this->rect.dim.y + this->fontHandle.TypeSize()) * 0.5f;
		break;

	case VTextAlign::Bottom:
		yPos = this->rect.pos.y + this->rect.dim.y;
		break;

	default:
		yPos = 0.0f;
		cvgAssert(false, "Unhandled VAlign calculation for UIText render.");
		break;
	}

	this->uiCols.norm.GLColor4();
	this->fontHandle.RenderFont(this->text.c_str(), xPos, yPos);
	return true;
}