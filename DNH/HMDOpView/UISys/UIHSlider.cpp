#include "UIHSlider.h"

UIHSlider::UIHSlider(UIBase* parent, int idx, float min, float max, float cur, const UIRect& r)
	: UIBase(parent, idx, r)
{
}

bool UIHSlider::Render()
{
	if(!this->selfVisible)
		return false;

	return true;
}