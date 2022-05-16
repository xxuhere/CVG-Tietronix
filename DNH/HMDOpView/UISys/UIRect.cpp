#include "UIRect.h"
#include <wx/glcanvas.h>

UIRect::UIRect()
{}

UIRect::UIRect(float px, float py, float w, float h)
{
	this->pos.x = px;
	this->pos.y = py;
	this->dim.x = w;
	this->dim.y = h;
}

UIRect::UIRect(const UIVec2& pos, const UIVec2& dim)
{
	this->pos = pos;
	this->dim = dim;
}

void UIRect::GLQuad()
{
	float right = this->pos.x + this->dim.x;
	float bottom = this->pos.y + this->dim.y;

	glBegin(GL_QUADS);
		glVertex2f(this->pos.x,		this->pos.y);
		glVertex2f(right,			this->pos.y);
		glVertex2f(right,			bottom );
		glVertex2f(this->pos.x,		bottom );
	glEnd();
}

void UIRect::GLQuadTex()
{
	float right = this->pos.x + this->dim.x;
	float bottom = this->pos.y + this->dim.y;

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(this->pos.x,		this->pos.y);
		//
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(right,			this->pos.y);
		//
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(right,			bottom);
		//
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(this->pos.x,		bottom);
	glEnd();
}