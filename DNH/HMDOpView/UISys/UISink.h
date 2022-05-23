#pragma once
#include "UIBase.h"
#include "UIVec2.h"

/// <summary>
/// An interface for a class to inherit to allow it to be a UISink. A UISink
/// can receive messages from a UISys when UI events happen in the UISys.
/// </summary>
class UISink
{
public:
	virtual void OnUISink_MouseDown(UIBase* uib, int mouseBtn, const UIVec2& mousePos);
	virtual void OnUISink_MouseUp(UIBase* uib, int mouseBtn, const UIVec2& mousePos);
	virtual void OnUISink_Clicked(UIBase* uib, int mouseBtn, const UIVec2& mousePos);
	virtual void OnUISink_MouseDrag(UIBase* uib, int mouseBtn, const UIVec2& mousePos);
	virtual void OnUISink_MouseHover(UIBase* uib, const UIVec2& mousePos);
	virtual void OnUISink_MouseExit(UIBase* uib);
	virtual void OnUISink_Keydown(UIBase* uib, int key);
	virtual void OnUISink_Keyup(UIBase* uib, int key);
	virtual void OnUISink_MouseMove(UIBase* uib, const UIVec2& mousePos);
	virtual void OnUISink_ChangeValue(UIBase* uib, float value, int vid);

	virtual void OnUISink_Show(UIBase* uib, bool show);

	//virtual void OnUISink_Activate();
	//virtual void OnUISink_Deactivate();
};