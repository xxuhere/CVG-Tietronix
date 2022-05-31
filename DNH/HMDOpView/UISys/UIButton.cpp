#include "UIButton.h"

UIButton::UIButton(UIBase* parent, int idx, const UIRect& r, const std::string& filePath)
	: UIGraphic(parent, idx, r, filePath)
{
	this->_InitializeColors();
}

UIButton::UIButton(UIBase* parent, int idx, const UIRect& r, TexObj::SPtr img)
	:	UIGraphic(parent, idx, r, img)
{
	this->_InitializeColors();
}

UIButton::UIButton(UIBase* parent, int idx, const UIRect& r, const std::string& text, int size, const std::string& fontType)
	: UIGraphic(parent, idx, r, TexObj::SPtr())
{
	this->SetMode_RawRect();

	this->text = text;

	if(fontType.empty())
		this->font = FontMgr::GetInstance().GetFont(size);
	else
		this->font = FontMgr::GetInstance().GetFont(fontType, size);

	this->_InitializeColors();
}

void UIButton::_InitializeColors()
{
	this->textColor.SetColor_Black();
	this->ResetColorsToDefault();
}

void UIButton::ResetColorsToDefault()
{
	this->uiCols.norm.Set(1.0f, 1.0f, 1.0f);
	this->uiCols.hover.Set(0.5f, 1.0f, 0.5f);
	this->uiCols.pressed.Set(0.75f, 1.0f, 0.75f);
}

void UIButton::HandleClick(int button)
{
	if(this->onClick)
		this->onClick(button);
}

bool UIButton::HandleSelectedWhiffDown(int button)
{
	// For now we always return true. Whether or not we want to
	// have it do something like auto-click the button, that
	// should probably be a setting in a variable/flag.
	return true;
}

bool UIButton::Render()
{
	if(!this->selfVisible)
		return false;

	this->_RenderGraphic();

	if(this->font.IsValid() && !this->text.empty())
	{ 
		this->textColor.GLColor3();

		this->font.RenderFontCenter(
			this->text.c_str(), 
			this->rect.Center(), 
			true);
	}

	this->UIBase::Render();
	return true;
}

int UpdateGroupColorSet( 
	int selIdx,
	std::initializer_list<UIButton*> elements,
	const ColorSetInteractable& colsHit,
	const ColorSetInteractable& colsOther)
{
	int ret = -1;
	int i = 0;
	for(UIButton* btn : elements)
	{
		if(btn->Idx() == selIdx)
		{
			ret = i;
			btn->uiCols = colsHit;
		}
		else
			btn->uiCols = colsOther;

		++i;
	}
	return i;
}