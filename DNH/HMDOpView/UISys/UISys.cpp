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

	if(mouseOver != nullptr)
	{
		// Left clicking selects
		if(this->IsSelectingButton(mouseButton)) 
		{
			if(mouseOver->IsSelectable())
			{
				this->ResetSelection();
				this->sel = mouseOver;
				this->sel->HandleSelect();
			}
			this->sel = mouseOver;
		}

		mouseOver->HandleMouseDown(pt, mouseButton);
		if(this->sink != nullptr)
			this->sink->OnUISink_MouseDown(mouseOver, mouseButton, pt);

		return DelMouseRet(
			DelMouseRet::Event::MouseDown,
			pt,
			mouseButton,
			mouseOver->Idx());
	}
	else
	{
		// Perform whiff click notification
		if(this->sel != nullptr)
		{ 
			if(this->sel->HandleSelectedWhiffDown(mouseButton))
			{
				// It needs to be decided whether we always dispatch a whiff notice, or only
				// do so when it's handled by the parent widget.
				if(this->sink != nullptr)
					this->sink->OnUISink_SelMouseDownWhiff(this->sel, mouseButton);

				return DelMouseRet(
					DelMouseRet::Event::MouseWhiffDown,
					pt,
					mouseButton,
					this->sel->idx);
			}
		}
	}

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
		if(this->sink != nullptr)
			this->sink->OnUISink_Clicked(mouseOver, mouseButton, pt);

		mouseOver->HandleClick(mouseButton);

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

std::vector<UIBase*> UISys::GetTabbingOrder()
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
	}

	return ret;
}

void UISys::AdvanceTabbingOrder(bool forward)
{
	std::vector<UIBase*> ret = this->GetTabbingOrder();
	if(ret.empty())
		return;

	// If nothing was selected, start the selection cycle.
	if(this->sel == nullptr)
	{ 
		this->sel = ret[0];
		this->sel->HandleSelect();
		return;
	}

	if(ret.size() == 1)
	{
		// If there's only one thing selected and it's already
		// selected, cycling (through the set of 1 thing) won't
		// do anything.
		if(ret[0] == this->sel)
			return;

		// If nothing known was selected, default to the first thing.
		this->sel->HandleUnselect();
		this->sel = ret[0];
		this->sel->HandleSelect();
		return;
	}
	
	// If nothing known was selection, start the selection cycle
	// by chosing the first thing.
	auto it = std::find(ret.begin(), ret.end(), this->sel);
	if (it == ret.end())
	{
		this->sel->HandleUnselect();
		this->sel = ret[0];
		this->sel->HandleSelect();
		return;
	}

	// Find the thing to progress to in the selection cycle based
	// off what's currently selected.
	int idx = it - ret.begin();
	if(forward)
		idx = (idx + 1) % (int)ret.size();
	else
		idx = ((idx - 1) + idx) % (int)ret.size();

	// Select it.
	this->sel->HandleUnselect();
	this->sel = ret[idx];
	this->sel->HandleSelect();
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