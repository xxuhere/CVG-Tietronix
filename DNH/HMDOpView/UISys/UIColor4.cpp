#include "UIColor4.h"

// Currently easier than seperating out Gl.h and its dependencies
// across platforms.
#include <wx/glcanvas.h> 

UIColor4::UIColor4()
{
	this->r = 1.0f;
	this->g = 0.0f;
	this->b = 1.0f;
	this->a = 1.0f;
}

UIColor4::UIColor4(float r, float g, float b)
{
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = 1.0f;
}

UIColor4::UIColor4(float r, float g, float b, float a)
{
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

void UIColor4::Set(float r, float g, float b)
{
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = 1.0f;
}

void UIColor4::Set(float r, float g, float b, float a)
{
	this->r = r; 
	this->g = g;
	this->b = b;
	this->a = a;
}

void UIColor4::GLColor3()
{
	glColor3fv(this->ar);
}

void UIColor4::GLColor4()
{
	glColor4fv(this->ar);
}