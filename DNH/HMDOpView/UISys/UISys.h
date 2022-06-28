#pragma once

#include "UIBase.h"
#include "UISink.h"
#include <wx/wx.h>

class UISink;

/// <summary>
/// Return value for mouse events.
/// </summary>
struct DelMouseRet
{
public:

	/// <summary>
	/// The event type.
	/// </summary>
	enum class Event
	{
		/// <summary>
		/// Mouse was moved.
		/// </summary>
		Moved,

		/// <summary>
		/// Mouse was hovered over a widget.
		/// </summary>
		Hovered,

		/// <summary>
		/// Mouse was pressed on a widget.
		/// </summary>
		MouseDown,

		/// <summary>
		/// Mouse was released on a widget.
		/// </summary>
		MouseUp,

		MouseWhiffDown,

		/// <summary>
		/// Mouse was pressed, but was not found to be on top of a 
		/// widget when that happened.
		/// </summary>
		MissedDown,

		/// <summary>
		/// Mouse was released, but was not found to be on top of a
		/// widget when that happened.
		/// </summary>
		MissedUp
	};

public:
	
	/// <summary>
	/// The event that was detected.
	/// </summary>
	Event evt;

	/// <summary>
	/// The mouse position.
	/// </summary>
	UIVec2 pt;

	/// <summary>
	/// The relevant mouse button.
	/// </summary>
	int button;

	/// <summary>
	/// The UI widget that was interacted with, or -1 if none was 
	/// found or if this variable is not relevant for the event.
	/// </summary>
	int idx;

public:
	
	DelMouseRet(Event evt, UIVec2 pt, int button, int idx);
};

/// <summary>
/// The root of the UI system. All UI hierarchies of UIBase objects
/// MUST have a UISys as a root to function properly as it holds the
/// state and mechanical logic to make everything work properly.
/// </summary>
class UISys : public UIBase
{
private:
	/// <summary>
	/// If true, show wireframe rectangles to show the boundaries 
	/// of ALL UIBase objects rendered.
	/// </summary>
	static bool showDebug;

protected:

	/// <summary>
	/// If set, it can be used to override navigation with a custom
	/// UI navigation order. Note that whoever sets this needs to
	/// ensure proper navigation.
	/// </summary>
	std::vector<UIBase*> customNav;

	/// <summary>
	/// The current UIBase in the hierarchy that is considered selected -
	/// and that has keyboard focus.
	/// </summary>
	UIBase* sel = nullptr;

public: 
	// TODO: these members need to be evaluated for proper encapsulation.

	/// <summary>
	/// An optional sink that can receive notifications of events when
	/// things happen in the UISys.
	/// </summary>
	UISink* sink = nullptr;

	/// <summary>
	/// The current UIBase in the hierarchy that the mouse was last over.
	/// Use to do state management of when mouse over and mouse exit
	/// events occur.
	/// </summary>
	UIBase* lastOver = nullptr;

	/// <summary>
	/// Track what was the mouse was over when the mouse was clicked,
	/// to allow for click drag.
	/// </summary>
	UIBase* onDown[3] = {nullptr, nullptr, nullptr};

	/// <summary>
	/// Specification of what mouse buttons can be considered clicks.
	/// 
	/// A bit-mask value of (1<<0) represent the left mouse.
	/// A bit-mask value of (1<<1) represents the middle mouse.
	/// A bit-mask value of (1<<2) represents the right mouse.
	/// </summary>
	int mouseBtnSelFlags = (1<<0);

public:
	UISys(int idx, const UIRect& r, UISink* sink);

protected:

	UISys* _GetSelfSys() override;

public:
	/// <summary>
	/// UIBase in the hierarchy should call this function in their
	/// root UISys when it is disabled - for proper state management.
	/// </summary>
	void _NotifyDisableChild(UIBase* widget);

	/// <summary>
	/// UIBase in the hierarchy should call this function in their
	/// root UISys when it is enabled - for proper state management.
	/// </summary>
	void _NotifyEnableChild(UIBase* widget);

	/// <summary>
	/// UIBase in the hierarchy should call this function in their
	/// root UISys when it is deleted - for proper state management.
	/// </summary>
	void _NotifyDeletedChild(UIBase* widget);

public:
	//////////////////////////////////////////////////
	//
	//		UTILITY FUNCTIONS
	//
	//////////////////////////////////////////////////

	void SubmitClick(UIBase* clickable, int button, const UIVec2& mousePos, bool sel);

	//////////////////////////////////////////////////
	//
	//		INTEGRATION FUNCTIONS
	//
	//////////////////////////////////////////////////
	// The functions below can-be/should-be called by outside code 
	// for the UI to properly function.

	/// <summary>
	/// This should be called in the application's Rendering code.
	/// 
	/// Unlike other implementations of UIBase::Render() where the
	/// rendering is expected to contained completly within the system,
	/// outside code is expected to call this function for the UISys
	/// in the application's rendering code.
	/// </summary>
	bool Render() override; 

	/// <summary>
	/// Check if dirty transforms need to be processed. This function
	/// should be called once per update/draw loop before drawing occurs.
	/// </summary>
	void AlignSystem();

	bool Select(UIBase* newSel);

	UIBase * GetSelected()
	{ return this->sel; }
	
	//		INPUT AND EVENT DELEGATION
	//////////////////////////////////////////////////
	// The Delegate* functions are used to pass inputs from the 
	// application to the UI system event handlers.

	bool DelegateKeydown(wxKeyCode key);
	bool DelegateKeyup(wxKeyCode key);
	DelMouseRet DelegateMouseDown(int mouseButton, const UIVec2& pt);
	DelMouseRet DelegateMouseUp(int mouseButton, const UIVec2& pt);
	DelMouseRet DelegateMouseMove(const UIVec2& pt);

	/// <summary>
	/// Clear all mouse-down state information.
	/// </summary>
	void DelegateReset();

	/// <summary>
	/// Clear the currently selected UI widget.
	/// </summary>
	void ResetSelection();

	std::vector<UIBase*> GetInnerTabbingOrder();
	void AdvanceTabbingOrder(bool forward);
	void AdvanceTabbingOrder(bool forward, const std::vector<UIBase*>& order);

	bool IsUsingCustomTabOrder() const 
	{return !this->customNav.empty(); }

	void ClearCustomTabOrder()
	{ return this->customNav.clear(); }

	void SetCustomTabOrder(std::vector<UIBase*> newOrder);

	//		IS SYS REGISTERED STATE QUERIES
	//////////////////////////////////////////////////

	/// <summary>
	/// Query if a mouse button is pressed on a specific UI widget.
	/// </summary>
	bool IsSysRegisteredMouseDown(int idx, const UIBase* uib) const;

	/// <summary>
	/// Query if any mouse buttons are pressed on a specific UI widget.
	/// </summary>
	bool IsSysRegisteredMouseDown(const UIBase* uib) const;

	/// <summary>
	/// Query if a UI widget is the UISys' selected widget.
	/// </summary>
	bool IsSysRegisteredSelected(const UIBase* uib) const;

	/// <summary>
	/// Set which mouse buttons can be used to select and interact with
	/// widgets.
	/// </summary>
	void SetSelectingButtons(bool left, bool middle, bool right);

	/// <summary>
	/// Check if a specific mouse button is set as a button that can
	/// select and interact with widgets.
	/// </summary>
	bool IsSelectingButton(int mouseBtn);

	//		DEBUGGING FUNCTIONS
	//////////////////////////////////////////////////

	/// <summary>
	/// Function to recursively draw the debug rectangles of the 
	/// UI hierarchy.
	/// </summary>
	void PlotDebugBoundsQuad() const;

	/// <summary>
	/// Check if debug drawing is on.
	/// </summary>
	static bool IsDebugView();

	/// <summary>
	/// Set the debug drawing state.
	/// </summary>
	static void ToggleDebugView(bool debug);

	/// <summary>
	/// Switch the debug drawing state.
	/// </summary>
	static void ToggleDebugView();
};

