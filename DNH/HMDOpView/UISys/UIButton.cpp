#include "UIButton.h"

UIButton::UIButton(int idx, const UIRect& r, const std::string& filePath)
	: UIBase(idx, r)
{
	this->image = TexObj::MakeSharedLODE(filePath);
}

UIButton::UIButton(int idx, const UIRect& r, TexObj::SPtr img)
	:	UIBase(idx, r),
		image(img)
{
}

void UIButton::Render()
{
	if(this->image.get() != nullptr)
	{ 
		glEnable(GL_TEXTURE_2D);
		this->image->GLBind();
	}
	glColor3f(1.0f, 1.0f, 1.0f);

	this->rect.GLQuadTex();

	UIBase::Render();
}