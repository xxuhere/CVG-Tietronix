#include "UISink.h"

// Do-nothing default implementations
void UISink::OnUISink_MouseDown(UIBase* uib, int mouseBtn, const UIVec2& mousePos){}
void UISink::OnUISink_MouseUp(UIBase* uib, int mouseBtn, const UIVec2& mousePos){}
void UISink::OnUISink_Clicked(UIBase* uib, int mouseBtn, const UIVec2& mousePos){}
void UISink::OnUISink_SelMouseDownWhiff(UIBase* uib, int mouseBtn){}
void UISink::OnUISink_MouseHover(UIBase* uib, const UIVec2& mousePos){}
void UISink::OnUISink_MouseExit(UIBase* uib){}
void UISink::OnUISink_MouseDrag(UIBase* uib, int mouseBtn, const UIVec2& mousePos){}
void UISink::OnUISink_Keydown(UIBase* uib, int key){}
void UISink::OnUISink_Keyup(UIBase* uib, int key){}
void UISink::OnUISink_MouseMove(UIBase* uib, const UIVec2& mousePos){}
void UISink::OnUISink_Show(UIBase* uib, bool show){}
void UISink::OnUISink_ChangeValue(UIBase* uib, float value, int vid){}