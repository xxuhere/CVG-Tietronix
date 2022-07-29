#include "UIPlate.h"
#include <wx/glcanvas.h>

UIPlate::UIPlate(UIBase* parent, int idx, const UIRect& r)
	: UIGraphic(parent, idx, r, TexObj::SPtr())
{
	this->uiCols.norm.Set(1.0f, 1.0f, 1.0f);
	this->uiCols.hover.Set(0.0f, 1.0f, 0.0f);
}

UIPlate::UIPlate(
	UIBase* parent, 
	int idx, 
	const UIRect& r, 
	const UIColor4& staticCol)
	: UIGraphic(parent, idx, r, TexObj::SPtr())
{
	this->uiCols.SetAll(staticCol);
}

bool UIPlate::IsSelectable()
{
	return false;
}