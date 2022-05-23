#include "UIButton.h"

UIButton::UIButton(UIBase* parent, int idx, const UIRect& r, const std::string& filePath)
	: UIBase(parent, idx, r)
{
	this->image = TexObj::MakeSharedLODE(filePath);

	this->_InitializeColors();
}

UIButton::UIButton(UIBase* parent, int idx, const UIRect& r, TexObj::SPtr img)
	:	UIBase(parent, idx, r),
		image(img)
{
	this->_InitializeColors();
}

UIButton::UIButton(UIBase* parent, int idx, const UIRect& r, const std::string& text, int size, const std::string& fontType)
	: UIBase(parent, idx, r)
{
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

bool UIButton::Render()
{
	if(!this->selfVisible)
		return false;

	UIColor4& col = 
		this->uiCols.GetContexedColor(
			this->pressedCt, 
			this->isHovered);
	
	col.GLColor4();

	if(this->image.get() != nullptr)
	{ 
		glEnable(GL_TEXTURE_2D);
		this->image->GLBind();
	}
	else
		glDisable(GL_TEXTURE_2D);

	this->rect.GLQuadTex();

	if(this->font.IsValid() && !this->text.empty())
	{ 
		this->textColor.GLColor3();

		this->font.RenderFontCenter(
			this->text.c_str(), 
			this->rect.Center(), 
			true);
	}

	return UIBase::Render();
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