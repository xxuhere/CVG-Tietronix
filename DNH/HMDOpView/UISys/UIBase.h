#pragma once
#include <vector>
#include "UIRect.h"

class UIBase
{
protected:
	bool dirty = true;
	bool visible = true;

	UIBase* parent = nullptr;
	std::vector<UIBase*> children;

	UIVec2 transPivot;
	UIVec2 transOffs;
	UIRect rect;

	int idx = -1;

public:
	UIBase(int idx, const UIRect& r);

	void FlagDirty();
	bool IsDirty(){return this->dirty;}

	void Show(bool show = true);
	inline void Hide(){this->Show(false);}
	inline bool IsVisible(){return this->visible;}

	void Destroy();
	void ClearChildren(bool destroy = true);
	void Align(bool recurse = true, bool force = false);

	void SetLocPos(const UIVec2& v);
	void SetLocPos(float x, float y);
	void SetLocXPos(float x);
	void SetLocYPos(float y);
	void SetPivot(const UIVec2& v);
	void SetPivot(float x, float y);
	void SetDim(const UIVec2& v);
	void SetDim(float x, float y);

	bool AddChild(UIBase* child);
	bool RemoveChild(UIBase* child);


	virtual void Render();

	virtual void OnEnabled();
	virtual void OnDisabled();

	~UIBase();
};