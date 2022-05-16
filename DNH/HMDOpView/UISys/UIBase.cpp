#include "UIBase.h"
#include "UIRect.h"
#include "../Utils/cvgAssert.h"

UIBase::UIBase(int idx, const UIRect& r)
{
	this->idx = idx;

	this->rect = r;

	this->transPivot = UIVec2(0.0f, 0.0f);
	this->transOffs = r.pos;
}

void UIBase::FlagDirty()
{
	this->dirty = true;

	if(this->parent)
		this->parent->FlagDirty();
}

void UIBase::Destroy()
{}

void UIBase::ClearChildren(bool destroy)
{
	if(destroy == true)
	{
		for(UIBase* child : this->children)
			delete child;
	}

	this->children.clear();
}

void UIBase::Render()
{
	for(UIBase* uib : this->children)
		uib->Render();
}

void UIBase::Align(bool recurse, bool force)
{
	if(!this->parent)
	{
		this->rect.pos = this->transOffs;
	}
	else
	{
		UIVec2 oldPos = this->parent->rect.pos;

		float fx = 
			this->parent->rect.pos.x + 
			this->transPivot.x * this->parent->rect.dim.x +
			this->transOffs.x;

		float fy =
			this->parent->rect.pos.y +
			this->transPivot.y * this->parent->rect.dim.y +
			this->transOffs.y;

		this->rect.pos.Set(fx, fy);

		force |= (this->rect.pos != oldPos);
	}

	this->dirty = false;

	if(recurse)
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
	this->transOffs.Set(x, y);
	this->FlagDirty();
}

void UIBase::SetLocXPos(float x)
{
	this->transOffs.x = x;
	this->FlagDirty();
}

void UIBase::SetLocYPos(float y)
{
	this->transOffs.y = y;
	this->FlagDirty();
}

void UIBase::SetPivot(const UIVec2& v)
{
	this->transPivot = v;
	this->FlagDirty();
}

void UIBase::SetPivot(float x, float y)
{
	this->transPivot.Set(x, y);
	this->FlagDirty();
}


void UIBase::SetDim(const UIVec2& v)
{
	this->rect.dim = v;
	this->FlagDirty();
}

void UIBase::SetDim(float x, float y)
{
	this->rect.dim.Set(x, y);
	this->FlagDirty();
}

void UIBase::Show(bool show)
{
	if(this->visible == show)
		return;
		
	if(show)
	{ 
		this->visible = true;
		this->FlagDirty();
	}
	else
	{
		this->visible = false;
	}
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

void UIBase::OnEnabled()
{}

void UIBase::OnDisabled()
{}

UIBase::~UIBase()
{
	this->ClearChildren(true);
}