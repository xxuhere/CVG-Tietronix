#pragma once
#include "UIBase.h"
#include "UIVec2.h"

class UISink
{
	virtual void OnUISink_Clicked(UIBase* uib, int mouseBtn, const UIVec2& mousePos) = 0;
};