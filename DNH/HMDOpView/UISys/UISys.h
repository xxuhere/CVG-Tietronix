#pragma once
#include "UIBase.h"
#include "UISink.h"
#include <wx/wx.h>

class UISink;

struct DelMouseRet
{
public:
	enum class Event
	{
		Moved,
		Hovered,
		MouseDown,
		MouseUp,
		MissedDown,
		MissedUp
	};

public:
	Event evt;
	UIVec2 pt;
	int button;
	int idx;

public:
	DelMouseRet(Event evt, UIVec2 pt, int button, int idx);
};

class UISys : public UIBase
{
private:
	static bool showDebug;

public:
	UISink* sink = nullptr;
	UIBase* sel = nullptr;
	UIBase* lastOver = nullptr;

	// Track what was the mouse was over when the mouse was clicked,
	// to allow for click drag.
	UIBase* onDown[3] = {nullptr, nullptr, nullptr};

public:
	UISys(int idx, const UIRect& r, UISink* sink);

	void PlotDebugBoundsQuad() const;

protected:
	UISys* _GetSelfSys();

public:
	// The class implementation
	bool Render() override; 

	void _NotifyDisableChild(UIBase* widget);
	void _NotifyEnableChild(UIBase* widget);
	void _NotifyDeletedChild(UIBase* widget);

public:
	bool DelegateKeydown(wxKeyCode key);
	bool DelegateKeyup(wxKeyCode key);
	DelMouseRet DelegateMouseDown(int mouseButton, const UIVec2& pt);
	DelMouseRet DelegateMouseUp(int mouseButton, const UIVec2& pt);
	DelMouseRet DelegateMouseMove(const UIVec2& pt);
	void DelegateReset();

	static void ToggleDebugView();

	void AlignSystem();

};

