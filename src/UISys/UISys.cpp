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
	glDisable(GL_BLEND);
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
		this->ResetSelection();

	if(widget == this->lastOver)
		this->lastOver = nullptr;
}

void UISys::SubmitClick(UIBase* clickable, int button, const UIVec2& mousePos, bool sel)
{
	if(sel)
		this->Select(clickable);

	if(this->sink != nullptr)
		this->sink->OnUISink_Clicked(clickable, button, mousePos);

	clickable->HandleClick(button);
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
		int ret = this->sel->HandleKeyUp(key);

		if(this->sink != nullptr)
			this->sink->OnUISink_Keydown(this->sel, key);

		return ret;
	}

	return false;
}

DelMouseRet UISys::DelegateMouseDown(int mouseButton, const UIVec2& pt)
{
	UIBase* mouseOver = this->CheckMouseOver(pt, false);

	if(this->onDown[mouseButton] != nullptr)
		this->onDown[mouseButton]->_RecordMouseRelease();

	this->onDown[mouseButton] = mouseOver;

	if(this->onDown[mouseButton] != nullptr)
		this->onDown[mouseButton]->_RecordMouseDown();

	return DelMouseRet(
		DelMouseRet::Event::MissedDown,
		pt,
		mouseButton,
		-1);
}

bool UISys::IsSysRegisteredMouseDown(int idx, const UIBase* uib) const
{
	if(uib == nullptr)
		return false;

	return this->onDown[idx] == uib;
}

bool UISys::IsSysRegisteredMouseDown(const UIBase* uib) const
{
	if(uib == nullptr)
		return false;

	// NOTE: We may just want to unroll the checks out of the loop.
	for(int i = 0; i < 3; ++i)
	{
		if(this->onDown[i] == uib)
			return true;
	}
	return false;
}

bool UISys::IsSysRegisteredSelected(const UIBase* uib) const
{
	if(uib == nullptr)
		return false;

	return this->sel == uib;
}

void UISys::SetSelectingButtons(bool left, bool middle, bool right)
{
	this->mouseBtnSelFlags = 0;

	if(left)
		this->mouseBtnSelFlags |= 1 << 0;

	if(middle)
		this->mouseBtnSelFlags |= 1 << 1;

	if(right)
		this->mouseBtnSelFlags |= 1 << 2;
}

bool UISys::IsSelectingButton(int mouseBtn)
{
	int andVal = 
		this->mouseBtnSelFlags & (1 << mouseBtn);

	return andVal != 0;
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

	UIBase* mouseOver = this->CheckMouseOver(pt, false);
	if(mouseOver != nullptr && prevDown == mouseOver)
	{ 
		this->SubmitClick(mouseOver, mouseButton, pt, false);

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
	this->ResetSelection();

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

void UISys::ResetSelection()
{
	if(this->sel != nullptr)
	{
		UIBase* oldSel = this->sel;
		this->sel = nullptr;
		oldSel->HandleUnselect();
	}
}

std::vector<UIBase*> UISys::GetInnerTabbingOrder()
{
	std::vector<UIBase*> ret;

	struct $_
	{
		static void AddChildren(UIBase* parent, std::vector<UIBase*>& colls)
		{
			for(int i = 0; i < parent->children.size(); ++i)
			{
				if(!parent->children[i]->IsSelfVisible())
					continue;

				if(!parent->children[i]->IsSelectable())
					continue;


				colls.push_back(parent->children[i]);
			}
		}
	};

	$_::AddChildren(this, ret);

	int idxIt = 0;
	while(idxIt < ret.size())
	{
		UIBase* uibIt = ret[idxIt];
		$_::AddChildren(uibIt, ret);
		++idxIt;
	}

	return ret;
}

void UISys::AdvanceTabbingOrder(bool forward)
{
	if(this->IsUsingCustomTabOrder())
	{ 
		this->AdvanceTabbingOrder(forward, this->customNav);
	}
	else
	{
		std::vector<UIBase*> innerOrder = this->GetInnerTabbingOrder();
		this->AdvanceTabbingOrder(forward, innerOrder);
	}
}

void UISys::AdvanceTabbingOrder(bool forward, const std::vector<UIBase*>& order)
{
	if(order.empty())
		return;

	// If nothing was selected, start the selection cycle.
	if(this->sel == nullptr)
	{ 
		this->Select(order[0]);
		return;
	}

	if(order.size() == 1)
	{
		// If there's only one thing selected and it's already
		// selected, cycling (through the set of 1 thing) won't
		// do anything.
		if(order[0] == this->sel)
			return;

		this->Select(order[0]);
		return;
	}

	// If nothing known was selection, start the selection cycle
	// by chosing the first thing.
	auto it = std::find(order.begin(), order.end(), this->sel);
	if (it == order.end())
	{
		this->Select(order[0]);
		return;
	}

	// Find the thing to progress to in the selection cycle based
	// off what's currently selected.
	int idx = it - order.begin();
	if(forward)
		idx = (idx + 1) % (int)order.size();
	else
		idx = ((idx - 1) + idx) % (int)order.size();

	this->Select(order[idx]);
}

void UISys::SetCustomTabOrder(std::vector<UIBase*> newOrder)
{
	// There might be more we need to do involving state-keeping, but for
	// now it's a simple/naive copy of values.

	this->customNav = newOrder;
}

UISys* UISys::_GetSelfSys()
{
	return this;
}

bool UISys::IsDebugView()
{
	return showDebug;
}

void UISys::ToggleDebugView(bool debug)
{
	showDebug = debug;
}

void UISys::ToggleDebugView()
{
	ToggleDebugView(!IsDebugView());
}



void UISys::AlignSystem()
{
	if(this->IsHierarchyDirty() || this->IsTransformDirty())
	{
		this->Align(
			true, 
			this->IsTransformDirty());
	}
}

bool UISys::Select(UIBase* newSel)
{
	if(newSel == nullptr)
	{
		if(!this->sel)
			return false;

		UIBase* oldSelClear = this->sel;
		this->sel = nullptr;
		oldSelClear->HandleUnselect();
		return true;
	}

	if(this->sel == newSel)
		return false;

	if(newSel->system != this)
		return false;

	UIBase* oldSel = this->sel;
	this->sel = newSel;

	if(oldSel)
		oldSel->HandleUnselect();

	if(this->sel)
		this->sel->HandleSelect();

	return true;
}