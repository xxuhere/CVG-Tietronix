#include "UIPlate.h"
#include <wx/glcanvas.h>

UIPlate::UIPlate(int idx, const UIRect& r)
	: UIBase(idx, r)
{}

void UIPlate::Render()
{
	glDisable(GL_TEXTURE_2D);
	glColor3f(1.0f, 0.0f, 1.0f);
	this->rect.GLQuad();

	UIBase::Render();
}