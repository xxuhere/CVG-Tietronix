#include "UIPlate.h"
#include <wx/glcanvas.h>

UIPlate::UIPlate(UIBase* parent, int idx, const UIRect& r)
	: UIBase(parent, idx, r)
{
	this->uiCols.norm.Set(1.0f, 1.0f, 1.0f);
	this->uiCols.hover.Set(0.0f, 1.0f, 0.0f);
}

bool UIPlate::Render()
{
	if(!this->selfVisible)
		return false;

	// If not filled, set to draw wireframe
	if(!this->filled)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDisable(GL_TEXTURE_2D);

	glColor3fv(this->uiCols.norm.ar);
	this->rect.GLQuad();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	return UIBase::Render();
}