#include "UISys.h"
#include <wx/glcanvas.h>

bool UISys::showDebug = true;

DelMouseRet::DelMouseRet(Event evt, UIVec2 pt, int button, int idx)
{
	this->evt		= evt;
	this->pt		= pt;
	this->button	= button;
	this->idx		= idx;
}

UISys::UISys(int idx, const UIRect& r, UISink* sink)
	:	UIBase(nullptr, idx, r)
{
	this->sink = sink;
}

void UISys::PlotDebugBoundsQuad() const
{
	glDisable(GL_TEXTURE);
	glColor3f(1.0f, 0.0f, 1.0f);
	this->_PlotDebugBoundsQuad();
}

bool UISys::Render()
{
	int ret = UIBase::Render();

	if(showDebug)
		PlotDebugBoundsQuad();

	return ret;
}

void UISys::_NotifyDisableChild(UIBase* widget)
{
	// If the UI
	UISys* root = widget->GetRootSys();
	if(root != this)
		return;

	// Sanity check, if the states are properly maintained,
	// this should always be true.
	if(widget == this->lastOver)
	{
		this->lastOver->HandleMouseExit();
		if(this->sink != nullptr)
			this->sink->OnUISink_MouseExit(lastOver);

		this->lastOver = nullptr;
	}
}

void UISys::_NotifyEnableChild(UIBase* widget)
{
	// Currently unused, but kept around just in case, and
	// for pairity with _NotifyDisableChild();
}

void UISys::_NotifyDeletedChild(UIBase* widget)
{
	UISys* root = widget->GetRootSys();
	if(root != this)
		return;

	if(widget == this->sel)
		this->sel = nullptr;

	if(widget == this->lastOver)
		this->lastOver = nullptr;
}

bool UISys::DelegateKeydown(wxKeyCode key)
{
	// Delegate to a child system, only if there is
	// an active UI element that has keyboard focus.
	if(this->sel != nullptr)
	{ 
		this->sel->HandleKeyDown(key);

		if(this->sink != nullptr)
			this->sink->OnUISink_Keydown(this->sel, key);

		return true;
	}

	return false;
}

bool UISys::DelegateKeyup(wxKeyCode key)
{
	// Right now we delegate regardless of if the key
	// was handled in OkKeydown() first. 
	//
	// This may need to change in the future.
	if(this->sel != nullptr)
	{ 
		return this->sel->HandleKeyUp(key);

		if(this->sink != nullptr)
			this->sink->OnUISink_Keydown(this->sel, key);
	}

	return false;
}

DelMouseRet UISys::DelegateMouseDown(int mouseButton, const UIVec2& pt)
{
	UIBase* mouseOver = this->CheckMouseOver(pt);

	if(this->onDown[mouseButton] != nullptr)
		this->onDown[mouseButton]->_RecordMouseRelease();

	this->onDown[mouseButton] = mouseOver;

	if(this->onDown[mouseButton] != nullptr)
		this->onDown[mouseButton]->_RecordMouseDown();

	if(mouseOver != nullptr)
	{
		// Left clicking selects
		if(mouseButton == 0) 
			this->sel = mouseOver;

		mouseOver->HandleMouseDown(pt, mouseButton);
		if(this->sink != nullptr)
			this->sink->OnUISink_MouseDown(mouseOver, mouseButton, pt);

		return DelMouseRet(
			DelMouseRet::Event::MouseDown,
			pt,
			mouseButton,
			mouseOver->Idx());
	}

	return DelMouseRet(
		DelMouseRet::Event::MissedDown,
		pt,
		mouseButton,
		-1);
}

DelMouseRet UISys::DelegateMouseUp(int mouseButton, const UIVec2& pt)
{

	UIBase* prevDown = this->onDown[mouseButton];
	if( prevDown != nullptr)
	{ 
		prevDown->HandleMouseUp(pt, mouseButton);
		prevDown->_RecordMouseRelease();

		if(this->sink != nullptr)
			this->sink->OnUISink_MouseUp(prevDown, mouseButton, pt);

		this->onDown[mouseButton] = nullptr;
	}

	UIBase* mouseOver = this->CheckMouseOver(pt);
	if(mouseOver != nullptr && prevDown == mouseOver)
	{ 
		if(this->sink != nullptr)
			this->sink->OnUISink_Clicked(mouseOver, mouseButton, pt);

		return DelMouseRet(
			DelMouseRet::Event::MouseUp,
			pt,
			mouseButton,
			idx);
	}

	return DelMouseRet(
		DelMouseRet::Event::MissedUp,
		pt,
		mouseButton,
		-1);
}

DelMouseRet UISys::DelegateMouseMove( const UIVec2& pt)
{
	UIBase* mouseOver = this->CheckMouseOver(pt, false);
	if(mouseOver != this->lastOver)
	{
		if(this->lastOver)
		{ 
			this->lastOver->HandleMouseExit();
			if(this->sink != nullptr)
				this->sink->OnUISink_MouseExit(mouseOver);
		}

		this->lastOver = mouseOver;

		if(this->lastOver)
		{ 
			this->lastOver->HandleMouseEnter(pt);
			if(this->sink != nullptr)
				this->sink->OnUISink_MouseHover(mouseOver, pt);
		}
	}

	for(int i = 0; i < 3; ++i)
	{
		if(this->onDown[i] == nullptr)
			continue;

		this->onDown[i]->HandleMouseDrag(pt, i);
		if(this->sink != nullptr)
			this->sink->OnUISink_MouseDrag(this->onDown[i], i, pt);
	}

	if(this->lastOver)
	{ 
		this->lastOver->HandleMouseMove(pt);
		if(this->sink)
			this->sink->OnUISink_MouseMove(this->lastOver, pt);
	}

	if(this->lastOver)
	{
		return DelMouseRet(
			DelMouseRet::Event::Hovered,
			pt,
			-1,
			this->lastOver->Idx());
	}
	else
	{
		return DelMouseRet(
			DelMouseRet::Event::Moved,
			pt,
			-1,
			-1);
	}
}

void UISys::DelegateReset()
{
	this->sel = nullptr;

	for(int i = 0; i < 3; ++i)
	{
		if(this->onDown[0] != nullptr)
			this->onDown[0]->_RecordMouseReset();

		this->onDown[0] = nullptr;
	}

	if(this->lastOver != nullptr)
	{
		this->lastOver->HandleMouseExit();
		if(this->sink != nullptr)
			this->sink->OnUISink_MouseExit(this->lastOver);

		this->lastOver = nullptr;
	}
}

UISys* UISys::_GetSelfSys()
{
	return this;
}

void UISys::ToggleDebugView()
{
	showDebug = !showDebug;
}