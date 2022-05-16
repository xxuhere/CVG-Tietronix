#include "UISys.h"

UISys::UISys(int idx, const UIRect& r)
	:	UIBase(idx, r)
{}

void UISys::Render(const wxPoint& pt, wxSize& sz)
{
	// !TODO: Set to self rect and do an internal render
	this->Render(UIRect(pt.x, pt.y, sz.x, sz.y));
}

void UISys::Render()
{
	this->Render(this->rect);
}

void UISys::Render(const UIRect& rgn)
{
	UIBase::Render();
}

bool UISys::OnKeydown(wxKeyCode key)
{
	return true;
}

bool UISys::OnMouseDown(int mouseButton, const UIVec2& pt)
{
	return true;
}

bool UISys::OnMouseUp(int mouseButton, const UIVec2& pt)
{
	return true;
}

bool UISys::OnHoverEnter( const UIVec2& pt)
{
	return true;
}

bool UISys::OnHoverExit( const UIVec2& pt)
{
	return true;
}

bool UISys::OnMouseMove( const UIVec2& pt)
{
	return true;
}