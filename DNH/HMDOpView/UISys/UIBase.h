#pragma once
#include <vector>
#include "UIRect.h"
#include "UIColor4.h"
#include "DynSize.h"
#include <string>

class UISys;

/// <summary>
/// Enumerated names for different mouse values.
/// </summary>
enum class MouseBtn
{
	// There's really no need to explicitly number these items
	// because they would have been enumerated these values
	// anyways, but I wanted to show that these specific values
	// are important.
	//
	// DO NOT RENUMBER OR REORDER

	/// <summary>
	/// Left click.
	/// </summary>
	Left	= 0 ,

	/// <summary>
	/// Middle click (mouse wheel)
	/// </summary>
	Middle	= 1,

	/// <summary>
	/// Right click
	/// </summary>
	Right	= 2
};

enum class HTextAlign
{
	/// <summary>
	/// Left align.
	/// </summary>
	Left,

	/// <summary>
	/// Middle align.
	/// </summary>
	Middle,

	/// <summary>
	/// Right align.
	/// </summary>
	Right
};

enum class VTextAlign
{
	/// <summary>
	/// Top align.
	/// </summary>
	Top,

	/// <summary>
	/// Center align.
	/// </summary>
	Center,

	/// <summary>
	/// Bottom align.
	/// </summary>
	Bottom
};

/// <summary>
/// The base class for all things in the UI hierarchy.
/// 
/// Note that the UIBase and it subclasses will often be synonomously
/// referred to as UI widgets.
/// </summary>
class UIBase
{
	friend class UISys;

public:
	/// <summary>
	/// Basic UI Colors.
	/// </summary>
	ColorSetInteractable uiCols;

	/// <summary>
	/// Not a functional variable. This can be arbitrarily set to more 
	/// easily track widgets in the watch list while step-through debugging.
	/// </summary>
	std::string debugName;

	/// <summary>
	/// A custom int ID associated with the widget. This is free to be used
	/// by outside code.
	/// </summary>
	int clientData = 0;

	/// <summary>
	/// A custom set of flags associated with the widget. This is free to be used
	/// byh outside code.
	/// </summary>
	int customFlags = 0;

protected:

	/// <summary>
	/// The explicit value of if the widget should be shown
	/// or not.
	/// </summary>
	bool visible = true;

	/// <summary>
	/// A cached recording of it the widget is shown. This
	/// should be equal to if visible is true, AS WELL AS all
	/// parent visible values.
	/// 
	/// - visible should be used to set the intended visible
	/// state for the widget.
	/// - selfVisible should be used to track if the heirarchy
	/// allows for the widget to be shown.
	/// </summary>
	bool selfVisible = true;

	/// <summary>
	/// Cache if the mouse of the parent UI system recognizes
	/// the widget as what the mouse is over.
	/// </summary>
	bool isHovered = false;

	/// <summary>
	/// Cache the number of times the mouse is currently clicking
	/// and dragging. The max value should be 3 - this is a little
	/// odd, be we track multiple clicks because the mouse has 3
	/// possible buttons to simultaneously click the widget with.
	/// 
	/// This is a pretty eccentric idea, and it debatable if we 
	/// want to track it this way, but for now since development
	/// is in flux, it's kept.
	/// </summary>
	int pressedCt = 0;

	/// <summary>
	/// The parent UISys, which should be the root UIBase of the
	/// hirarchy.
	/// </summary>
	UISys* system = nullptr;

	/// <summary>
	/// The widget's immediate parent.
	/// </summary>
	UIBase* parent = nullptr;

	/// <summary>
	/// Optional dynamic resize behaviour data.
	/// </summary>
	DynSize* dyn = nullptr;

	/// <summary>
	/// Child widgets in the UI hierarchy.
	/// </summary>
	std::vector<UIBase*> children;

	/// <summary>
	/// 
	/// </summary>
	UIVec2 transPivot;

	/// <summary>
	/// Local transform offset.
	/// </summary>
	UIVec2 transOffs;

	/// <summary>
	/// Global transform rect. This is the final boundary that will
	/// be used when drawing the UI to the screen.
	/// </summary>
	UIRect rect;

	/// <summary>
	/// The widget ID. This can be set arbitrarily to anything to track
	/// Set to -1 if unused. This value will be used to  identify the 
	/// widgets.
	/// </summary>
	int idx = -1;

private:
	/// <summary>
	/// Bitflag for if a child in its transform is dirty. Note that if an
	/// operation leave the hierarchy ambiguous, the dirty flag should be
	/// overly-conservative on setting it to dirty - i.e., better to err on
	/// "over-processing nothing, rather than ignoring something".
	/// </summary>
	bool dirtyHierarchy : 1;

	/// <summary>
	/// Bitflag for if the transform of the UI base is dirty.
	/// </summary>
	bool dirtyTransform : 1;

protected:
	/// <summary>
	/// Bitflag for if the contents of the actual UIBase is dirty, which
	/// is orthogonal to if the transform is dirty.
	/// 
	/// What "contents" actually means, will depend on the subclass
	/// implementation.
	/// </summary>
	bool dirtyContents	: 1;

protected:

	//////////////////////////////////////////////////
	//
	//	Special utility functions. 
	//
	// These should not be used except in special locations where
	// they are already fully integrated. Do not adding in other
	// places.
	//
	//////////////////////////////////////////////////

	/// <summary>
	/// Find the root UISys and save it to this->system.
	/// </summary>
	void _RecacheSelfSys();

	/// <summary>
	/// Used to notify the widget that is has been clicked. It should be
	/// expected that a _RecordMouseRelease() even will be sent as soon
	/// as the mouse button is released.
	/// </summary>
	void _RecordMouseDown();

	/// <summary>
	/// Used to notify the widget that the widget is not longer clicked on
	/// from a previously recorded mouse button down.
	/// </summary>
	void _RecordMouseRelease();

	/// <summary>
	/// Used to notify the widget that all mouse buttons clicks have been
	/// released from an interrupting event.
	/// </summary>
	void _RecordMouseReset();

	/// <summary>
	/// Draw the bounds as a GLQuad. Used to debugging outlines.
	/// </summary>
	void _PlotDebugBoundsQuad() const;

	/// <summary>
	/// Used to manage propagating show events through the hierarchy.
	/// </summary>
	/// <param name="show"></param>
	void _InternalShow(bool show);

protected:

	// Only UISys implementation will return non-nullpr
	virtual UISys* _GetSelfSys();

	/// <summary>
	/// Can be implemented to specify logic and behaviour when the widget's
	/// visibility state changes to being shown.
	/// </summary>
	virtual void OnEnabled();

	/// <summary>
	/// Can be implemented to specify logic and behaviour when the widget's
	/// visibility state changes to being hidden.
	/// </summary>
	virtual void OnDisabled();

	/// <summary>
	/// Called whenever the transform is readjusted from an Align.
	/// 
	/// It is suggested that the implementation flag handling the
	/// state change for later instead of immediately handling the
	/// state change, in case multiple aligns are called.
	/// </summary>
	virtual void OnAligned();

	/// <summary>
	/// Callback for when the mouse is moved over the widget (when it previously
	/// wasn't).
	/// </summary>
	/// <param name="pos">The mouse position, in screen coordinates.</param>
	virtual void HandleMouseEnter(const UIVec2& pos);

	/// <summary>
	/// Callback for when an entered widget (see HandleMouseEnter()) has the 
	/// mouse moved off the widget.
	/// </summary>
	virtual void HandleMouseExit();

	/// <summary>
	/// Callback for when the mouse clicks on the widget.
	/// </summary>
	/// <param name="pos">The mouse position, in screen coordinates.</param>
	/// <param name="button">The button the was clicked.</param>
	virtual void HandleMouseDown(const UIVec2& pos, int button);

	/// <summary>
	/// Callback for when the mouse is released after clicking
	/// on the widget.
	/// </summary>
	/// <param name="pos">The mouse position in screen coordinates.</param>
	/// <param name="button">The button the was released.</param>
	virtual void HandleMouseUp(const UIVec2& pos, int button);

	/// <summary>
	/// Callback for when the UI widget is clicked. For a click to occur, the
	/// mouse needs to first receive a MouseDown, and then later receive a
	/// MouseUp while on top of the original widget.
	/// </summary>
	/// <param name="button">The mouse button used to click the widget.</param>
	virtual void HandleClick(int button);

	/// <summary>
	/// Callback for when the mouse is moved over the widget. These
	/// callbacks will be received between a HandleMouseEnter() and
	/// HandleMouseExit() call.
	/// </summary>
	/// <param name="pos">The position of the mouse, in screen coordinates.</param>
	virtual void HandleMouseMove(const UIVec2& pos);

	/// <summary>
	/// Callback for when the mouse is being dragged. Note that if multiple
	/// mouse buttons are dragging the widget, multiple calls with different
	/// button parameters will be called.
	/// 
	/// This will be send every time the mouse is moved when betwen 
	/// HandleMouseDown() and HandleMouseUp().
	/// </summary>
	/// <param name="pos">The mouse position, in screen coordinates.</param>
	/// <param name="button">The button dragging the widget.</param>
	virtual void HandleMouseDrag(const UIVec2& pos, int button);
	
	/// <summary>
	/// Callback for when the widget is the last thing the mouse clicked on (in the
	/// UISys hierarchy) and a key was pressed.
	/// </summary>
	/// <param name="keycode">The key pressed. This will be a wxWidget keycode.</param>
	/// <returns>If handled, return true.</returns>
	virtual bool HandleKeyDown(int keycode);

	/// <summary>
	/// Callback for when the keypress from a previous HandleKeyDown() is released.
	/// </summary>
	/// <param name="keycode">The key being released.</param>
	/// <returns>If handled, return true.</returns>
	virtual bool HandleKeyUp(int keycode);

	/// <summary>
	/// Callback for when the UI widget is selected.
	/// </summary>
	virtual void HandleSelect();

	/// <summary>
	/// Callback for when the UI widget, when previously selected, is unselected.
	/// </summary>
	virtual void HandleUnselect();

	virtual bool HandleSelectedWhiffDown(int button);

	/// <summary>
	/// Get a float value from the widget. The types of values supports, their ids, 
	/// and the format of the value will depend on the subclass.
	/// </summary>
	/// <param name="vid"></param>
	/// <returns>The value of the id.</returns>
	virtual float GetValue(int vid);

public:
		/// <summary>
		/// Query if the UI widget can be selected when interacted with the mouse.
		/// 
		/// NOTE: This may also be relevant if there's a tab navigation feature - which 
		/// currently doesn't exist.
		/// </summary>
		/// <returns></returns>
		virtual bool IsSelectable();

public:
	/// <summary>
	/// Constructor.
	/// </summary>
	/// <param name="parent">The parent widget.</param>
	/// <param name="idx">
	/// The widget id. This can be any value you want to use, identify
	/// the widget. To ignore this value, use -1.
	/// </param>
	/// <param name="r">
	/// The local position and dimension. If this value is empty, chances
	/// are it's because the dynamic sizing is used instead.
	/// </param>
	UIBase(UIBase* parent, int idx, const UIRect& r);

	/// <summary>
	/// Given a mouse position, find the widget in the hierarchy that the
	/// position is on top of.
	/// </summary>
	/// <param name="pos">The mouse position, in screen coordinates.</param>
	/// <param name="testSelf">
	/// If true, the invoking object itself can be a return value. Else,
	/// only children can be returned.
	/// </param>
	/// <returns>
	/// The deepest widget that the mouse was found to be over. nullptr if
	/// nothing was found.
	/// </returns>
	UIBase* CheckMouseOver(const UIVec2& pos, bool testSelf = true);

	inline int Idx() const {return this->idx;};

	inline bool HasCustomFlags(int flags) const
	{ return (this->customFlags & flags) != 0; }

	void FlagTransformDirty(bool flagHierarchy = true);
	void FlagHierarchyDirty();
	inline void FlagContentsDirty() { this->dirtyContents = true; }

	bool IsHierarchyDirty() const { return this->dirtyHierarchy; }
	bool IsTransformDirty() const { return this->dirtyTransform; }
	bool IsContentsDirty() const { return this->dirtyContents; }

	/// <summary>
	/// Set all colors in the color settings to the same color.
	/// </summary>
	/// <param name="col">The color to set the UI widget.</param>
	void SetAllColors(const UIColor4& col);

	inline bool IsSelfVisible() const
	{ return this->selfVisible; }

	/// <summary>
	/// Check if the UIBase is registered to a UISys, and if it
	/// has a specific mouse button down.
	/// </summary>
	/// <param name="idx">The mouse button to check for.</param>
	/// <returns>
	/// If true, the root UISys recognizes that the specific mouse button
	/// is down for the invoking object.
	/// </returns>
	bool IsRegisteredMouseDown(int idx) const;

	/// <summary>
	/// Check if the UIBase is registered to a UISys, and if it
	/// has any mouse buttons down.
	/// </summary>
	/// <returns>
	/// If true, the root UISys recognizes that a mouse button is down
	/// for the invoking object.
	/// </returns>
	bool IsRegisteredMouseDown() const;

	/// <summary>
	/// Check if the UIBase is registered to a UISys, and if it
	/// is the UISys's currently selected UIBase.
	/// </summary>
	/// <returns>If true, the root UISys recognizes that the 
	/// invoking object is the selected object.</returns>
	bool IsRegisteredSelected() const;

	/// <summary>
	/// Show the widget.
	/// </summary>
	/// <param name="show">
	/// If true, the widget is set to be shown. 
	/// Else if false, the widget is set to be hidden.
	/// </param>
	void Show(bool show = true);

	/// <summary>
	/// Hide the widget.
	/// </summary>
	inline void Hide() {this->Show(false);}

	inline bool IsVisible() {return this->selfVisible;}

	/// <summary>
	/// Destroy the widget and its hierarchy.
	/// </summary>
	void Destroy();

	/// <summary>
	/// Destroy the widget's hierarchy, but don't delete
	/// the widget itself.
	/// </summary>
	void ClearChildren();

	/// <summary>
	/// Realign the nodes
	/// </summary>
	/// <param name="scanRebuild">
	/// Recurse and look for nodes that may have
	/// </param>
	/// <param name="forceRebuild">
	/// Force rebuilding the node and the entire child hierarchy.
	/// This will be set to true if we know a parent node's transform
	/// was previously modified.
	/// </param>
	void Align(bool scanRebuild, bool forceRebuild);

	// Transform modification functions. Used to set the position
	// of the UI elements, as well as flag the transform as dirty.
	//
	void SetLocPos(const UIVec2& v);
	void SetLocPos(float x, float y);
	void SetLocXPos(float x);
	void SetLocYPos(float y);
	void SetPivot(const UIVec2& v);
	void SetPivot(float x, float y);
	void SetDim(const UIVec2& v);
	void SetDim(float x, float y);
	void SetRect(const UIRect& r);
	void SetRect(float x, float y, float w, float h);

	/// <summary>
	/// Add a child to the widget's UI hierarchy.
	/// </summary>
	/// <param name="child"></param>
	/// <returns></returns>
	bool AddChild(UIBase* child);

	/// <summary>
	/// Remove a child from the widget's UI hierarchy.
	/// </summary>
	/// <param name="child"></param>
	/// <returns></returns>
	bool RemoveChild(UIBase* child);

	/// <summary>
	/// Send a value to the system sink. Used to pass values and
	/// change notifications to the application.
	/// </summary>
	/// <param name="value">The new value.</param>
	/// <param name="vid">
	/// The value ID. Exactly what this means depends on the subclass.
	/// If the subclass only has 1 type of value, this will be ignored.
	/// </param>
	void SubmitValue(float value, int vid);

	inline int ChildCt() const
	{ return (int)this->children.size(); }

	inline UIBase* GetChild(int idx)
	{ return this->children[idx]; }

	/// <summary>
	/// Enable using dynamic sizing data and get access to the 
	/// allocated dynamic sizing data.
	/// </summary>
	DynSize* UseDyn();

	/// <summary>
	/// Disable using dynamic sizing data.
	/// </summary>
	void DisableDyn();

	/// <summary>
	/// The class should implement this to render the widget.
	/// 
	/// If children should be rendered, make sure to call
	/// this->UIBase::Render() at the end of the function.
	/// 
	/// The function should return true if anything was
	/// rendered. The function is also in charge of checking
	/// if this->selfVisible is true or not. If not, immediately
	/// return false. For design reasons, this was not automated.
	/// </summary>
	/// <returns>true if anything was rendered. false if the
	/// widget is hidden or not rendering anything.</returns>
	virtual bool Render();
	
	/// <summary>
	/// Cast the UIBase to a UISys. All implementations of UIBase
	/// should return nullptr except for UISys subclasses. This is
	/// done as a hockey way to avoid RTTI casts.
	/// </summary>
	virtual UISys* GetRootSys();

	~UIBase();
};