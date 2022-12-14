#include "UIColor4.h"

// Currently easier than seperating out Gl.h and its dependencies
// across platforms.
#include <wx/glcanvas.h> 

static UIColor4 _Black;
static UIColor4 _White;
static UIColor4 _Red;
static UIColor4 _Green;
static UIColor4 _Blue;


const UIColor4& UIColor4::Black = _Black;
const UIColor4& UIColor4::White = _White;
const UIColor4& UIColor4::Red	= _Red;
const UIColor4& UIColor4::Green = _Green;
const UIColor4& UIColor4::Blue	= _Blue;

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

void UIColor4::ModulateSelf(const UIColor4& other)
{
	this->r *= other.r; 
	this->g *= other.g;
	this->b *= other.b;
	this->a *= other.a;
}

UIColor4 UIColor4::Modulate(const UIColor4& other) const
{
	return UIColor4(
		this->r * other.r,
		this->g * other.g,
		this->b * other.b,
		this->a * other.a);
}

bool UIColor4::operator== (const UIColor4& o) const
{
	return 
		this->r == o.r && 
		this->g == o.g && 
		this->b == o.b && 
		this->a == o.a;
}

ColorSetInteractable::ColorSetInteractable()
{}

ColorSetInteractable::ColorSetInteractable(
	const UIColor4& norm, 
	const UIColor4& hover, 
	const UIColor4& pressed)
{
	this->norm		= norm;
	this->sel		= norm;
	this->hover		= hover;
	this->pressed	= pressed;
}

ColorSetInteractable::ColorSetInteractable(const UIColor4& all)
{
	this->norm		= all;
	this->sel		= all;
	this->hover		= all;
	this->pressed	= all;
}

void ColorSetInteractable::Set(
	const UIColor4& norm, 
	const UIColor4& hover, 
	const UIColor4& pressed)
{
	this->norm		= norm;
	this->sel		= norm;
	this->hover		= hover;
	this->pressed	= pressed;
}

void ColorSetInteractable::Set(
	const UIColor4& norm, 
	const UIColor4& hover, 
	const UIColor4& pressed,
	const UIColor4& selected)
{
	this->norm		= norm;
	this->hover		= hover;
	this->pressed	= pressed;
	this->sel		= selected;
}

ColorSetInteractable::ColorSetInteractable(
	const UIColor4& norm, 
	const UIColor4& hover, 
	const UIColor4& pressed,
	const UIColor4& selected)
{
	this->norm		= norm;
	this->hover		= hover;
	this->pressed	= pressed;
	this->sel		= selected;
}

void ColorSetInteractable::SetAll(const UIColor4& all)
{
	this->norm		= all;
	this->hover		= all;
	this->pressed	= all;
}

UIColor4& ColorSetInteractable::GetContexedColor(int pressedCt, bool isHovering, bool isSel)
{
	if(pressedCt != 0)
		return this->pressed;

	if(isHovering)
		return this->hover;
	
	if(isSel)
		return this->sel;

	return this->norm;
}