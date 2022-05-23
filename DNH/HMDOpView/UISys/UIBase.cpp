#include "UIBase.h"
#include "UIRect.h"
#include "UISys.h"
#include "../Utils/cvgAssert.h"
#include <wx/glcanvas.h>

UISys* UIBase::_GetSelfSys()
{
	return nullptr;
}

UIBase::UIBase(UIBase* parent, int idx, const UIRect& r)
{
	this->idx = idx;

	this->rect = r;

	this->transPivot = UIVec2(0.0f, 0.0f);
	this->transOffs = r.pos;

	if(parent != nullptr)
		parent->AddChild(this);
}

void UIBase::_RecacheSelfSys()
{
	this->system = this->_GetSelfSys();
}

void UIBase::_RecordMouseDown()
{
	++this->pressedCt;
}

void UIBase::_RecordMouseRelease()
{
	this->pressedCt = std::max(this->pressedCt - 1, 0);
}

void UIBase::_RecordMouseReset()
{
	this->pressedCt = 0;
}

void UIBase::_PlotDebugBoundsQuad() const
{
	if(!this->selfVisible)
		return;

	glBegin(GL_LINE_LOOP);
		glVertex2f(
			this->rect.pos.x, 
			this->rect.pos.y);
		glVertex2f(
			this->rect.pos.x + this->rect.dim.x, 
			this->rect.pos.y);
		glVertex2f(
			this->rect.pos.x + this->rect.dim.x,
			this->rect.pos.y + this->rect.dim.y);
		glVertex2f(
			this->rect.pos.x,
			this->rect.pos.y + this->rect.dim.y);
	glEnd();

	for(UIBase* child : this->children)
		child->_PlotDebugBoundsQuad();
}

UIBase* UIBase::CheckMouseOver(const UIVec2& pos, bool testSelf)
{
	if(testSelf)
	{
		if(!this->selfVisible || !UIVec2::InRectBounds(pos, this->rect))
			return nullptr;
	}

	// Delegate
	for(int i = this->children.size() - 1; i >= 0; --i)
	{
		// If intercepted by child, they get the exclusive sibbling
		// handle message
		UIBase* ret = this->children[i]->CheckMouseOver(pos);
		if(ret != nullptr)
			return ret;
	}

	return testSelf ? this : nullptr;
}

void UIBase::FlagDirty()
{
	this->dirty = true;

	if(this->parent)
		this->parent->FlagDirty();
}

void UIBase::Destroy()
{}

void UIBase::ClearChildren()
{
	for(UIBase* child : this->children)
		delete child;

	this->children.clear();
}

bool UIBase::Render()
{
	if(!this->selfVisible)
		return false;

	for(UIBase* uib : this->children)
		uib->Render();

	return true;
}

void UIBase::Align(bool recurse, bool force)
{
	if(!force && !this->dirty)
		return;

	// If not visible, there's no point to aligning it right now. It will still
	// need to be Aligned later when made visible, but Show()ing it later should
	// reset the dirty flag and trigger another Align with it one.
	if(this->selfVisible == false)
		return;


	UIRect oldR = this->rect;

	if(!this->parent)
	{
		this->rect.pos = this->transOffs;
	}
	else if(this->dyn && this->dyn->active)
	{
		UIVec2 tl = this->parent->rect.pos + this->parent->rect.dim * this->dyn->anchMin;
		UIVec2 br = this->parent->rect.pos + this->parent->rect.dim * this->dyn->anchMax;
		tl += this->dyn->offsMin;
		br += this->dyn->offsMax;

		this->rect.pos = tl;
		this->rect.dim = br - tl;
	}
	else
	{
		UIVec2 oldPos = this->rect.pos;

		float fx = 
			this->parent->rect.pos.x + 
			this->transPivot.x * this->parent->rect.dim.x +
			this->transOffs.x;

		float fy =
			this->parent->rect.pos.y +
			this->transPivot.y * this->parent->rect.dim.y +
			this->transOffs.y;

		this->rect.pos.Set(fx, fy);
	}

	this->dirty = false;

	bool modified = (this->rect != oldR);
	if(modified || recurse)
	{
		for(UIBase* uib : this->children)
		{
			if(!uib->IsDirty() && !force)
				continue;

			uib->Align(true);
		}
	}
}

void UIBase::SetLocPos(const UIVec2& v)
{
	this->transOffs = v;
	this->FlagDirty();
}

void UIBase::SetLocPos(float x, float y)
{
	if(this->transOffs.x == x && this->transOffs.y == y)
		return;

	this->transOffs.Set(x, y);
	this->FlagDirty();
}

void UIBase::SetLocXPos(float x)
{
	if(this->transOffs.x == x)
		return;

	this->transOffs.x = x;
	this->FlagDirty();
}

void UIBase::SetLocYPos(float y)
{
	if(this->transOffs.y == y)
		return;

	this->transOffs.y = y;
	this->FlagDirty();
}

void UIBase::SetPivot(const UIVec2& v)
{
	if(this->transPivot == v)
		return;

	this->transPivot = v;
	this->FlagDirty();
}

void UIBase::SetPivot(float x, float y)
{
	if(this->transPivot.x == x && this->transPivot.y == y)
		return;

	this->transPivot.Set(x, y);
	this->FlagDirty();
}


void UIBase::SetDim(const UIVec2& v)
{
	if(this->rect.dim == v)
		return;

	this->rect.dim = v;
	this->FlagDirty();
}

void UIBase::SetDim(float x, float y)
{
	if(this->rect.dim.x == x && this->rect.dim.y == y)
		return;

	this->rect.dim.Set(x, y);
	this->FlagDirty();
}

void UIBase::SetRect(const UIRect& r)
{
	if(this->rect == r)
		return;

	this->transOffs = r.pos;
	this->rect.dim = r.dim;
	this->FlagDirty();
}

void UIBase::SetRect(float x, float y, float w, float h)
{
	this->SetRect(UIRect(x, y, w, h));
}

bool UIBase::AddChild(UIBase* child)
{
	cvgAssert(child != this, "Cannot add self as UIBase child");
	// We current trust the call to not create a corrupt
	// hierarchy.

	if(child->parent == this)
		return false;

	if(child->parent != nullptr)
		child->parent->RemoveChild(child);

	this->children.push_back(child);
	child->parent = this;

	child->FlagDirty();

	this->_RecacheSelfSys();
	return true;
}

bool UIBase::RemoveChild(UIBase* child)
{
	for(int i = 0; i < this->children.size(); ++i)
	{
		if(this->children[i] == child)
		{
			this->children.erase(this->children.begin() + i);
			child->parent = nullptr;
			child->FlagDirty();
			return true;
		}
	}
	return false;
}

void UIBase::SubmitValue(float value, int vid)
{
	if(this->system != nullptr)
	{
		if(this->system->sink != nullptr)
			this->system->sink->OnUISink_ChangeValue(this, value, vid);
	}
}

DynSize* UIBase::UseDyn()
{
	this->FlagDirty();

	if(this->dyn == nullptr)
		this->dyn = new DynSize();

	this->dyn->active = true;
	return this->dyn;
}

void UIBase::DisableDyn()
{
	this->FlagDirty();

	if(this->dyn != nullptr)
		this->dyn->active = false;
}

void UIBase::_InternalShow(bool show)
{
	bool wasChanged = (this->selfVisible != show);
	this->selfVisible = show;

	if(show == false)
	{ 
		// If the hierarchy is of, absolutely everything
		// in the hierarchy gets turned off with it...
		for(UIBase* c : children)
			c->_InternalShow(false);
	}
	else
	{
		// But if showing the hierarchy, only children set
		// to be shown will be shown
		for(UIBase* c : children)
		{
			if(c->visible)
				c->_InternalShow(true);
		}
	}

	if(wasChanged)
	{
		this->FlagDirty();

		if(show)
		{ 
			this->OnEnabled();
			if(this->system != nullptr)
				this->system->_NotifyEnableChild(this);
		}
		else
		{ 
			this->OnDisabled();
			if(this->system != nullptr)
				this->system->_NotifyDisableChild(this);
		}
	}
}

void UIBase::Show(bool show)
{
	this->visible = show;
	_InternalShow(show);
}


void UIBase::OnEnabled()
{}

void UIBase::OnDisabled()
{}

// Mouse handling functions will typically be iterated in reverse
// order. This is because when we draw things, things at the top
// get priority to be handled by the mouse before things at the bottom -
// and things at the bottom were drawn first (earlier in the child
// order).

void UIBase::HandleMouseEnter(const UIVec2& pos)
{
	this->isHovered = true;
}

void UIBase::HandleMouseExit()
{
	this->isHovered = false;
}

void UIBase::HandleMouseDown(const UIVec2& pos, int button)
{}

void UIBase::HandleMouseDrag(const UIVec2& pos, int button)
{}

void UIBase::HandleMouseUp(const UIVec2& pos, int button)
{}

void UIBase::HandleMouseMove(const UIVec2& pos)
{}

bool UIBase::HandleKeyDown(int keycode)
{
	return false;
}

bool UIBase::HandleKeyUp(int keycode)
{
	return false;
}

UISys* UIBase::GetRootSys()
{
	// Get root element
	UIBase* it = this;
	while(it->parent != nullptr)
		it = it->parent;

	// It should be a UISys, but use the built-in casting
	// to be safe.
	return this->_GetSelfSys();
}

UIBase::~UIBase()
{
	if(this->system)
	{
		this->system->_NotifyDeletedChild(this);
		this->system = nullptr;
	}

	if(dyn)
	{
		delete this->dyn;
		this->dyn = nullptr;
	}

	this->ClearChildren();
}