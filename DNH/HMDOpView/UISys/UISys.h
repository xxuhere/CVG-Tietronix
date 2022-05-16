#pragma once
#include "UIBase.h"
#include <wx/wx.h>

class UISys : public UIBase
{
public:
	UISys(int idx, const UIRect& r);

	void Render(const wxPoint& pt, wxSize& sz);
	void Render(const UIRect& rgn);
	void Render() override;

public:
	bool OnKeydown(wxKeyCode key);
	bool OnMouseDown(int mouseButton, const UIVec2& pt);
	bool OnMouseUp(int mouseButton, const UIVec2& pt);
	bool OnHoverEnter(const UIVec2& pt);
	bool OnHoverExit(const UIVec2& pt);
	bool OnMouseMove(const UIVec2& pt);
};