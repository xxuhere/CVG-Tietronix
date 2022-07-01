#pragma once

#include "../Substate.h"
#include <vector>

class StateHMDOp;
class UIBase;
class UISys;

/// <summary>
/// UI Substate to manage input in the context of when
/// the StateHMDOp is showing a submenu that has button groups.
/// </summary>
class HMDOpSub_InspNavForm : public Substate<StateHMDOp>
{
	/// <summary>
	/// The inspector plate holding the UI Widget contents that this
	/// state is responsible for navigating. This is held so we know
	/// what submenu to close when this state is exited. 
	/// 
	/// NOTE: Depending on how we decide the application should work, 
	/// submenu may not always be forced to close.
	/// </summary>
	UIBase* inspectorPlate;

	/// <summary>
	/// The main menu button that invoked this state. This is cached
	/// so that is can be reselected when the state is popped.
	/// </summary>
	UIBase* optButton;

	/// <summary>
	/// A cache of the UI widgets that can be navigated.
	/// </summary>
	std::vector<UIBase*> widgets;

public:

	/// <summary>
	/// Given a UIWidget that's expected to be parent that has selectable children,
	/// gather a collection of all the selectable children in its hierarchy. Add
	/// items in order, favoring depth-first traversal collection.
	/// </summary>
	/// <param name="root">The parent widget to collect the children of.</param>
	/// <param name="vecw">The output of the collected items.</param>
	static void CollectSelectable(UIBase* root, std::vector<UIBase*>& vecw);

	HMDOpSub_InspNavForm(UIBase* optButton, UIBase* inspPlate);
	void OnLeftUp(			StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override; 
	void OnMiddleUp(		StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnMiddleUpHold(	StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnRightUp(			StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override; 
	void OnEnterContext(	StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override; 
	void OnExitContext(		StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	std::string GetStateName() const override;

	/// <summary>
	/// Advance focus to the next widget group. i.e.:
	/// 
	/// If something in widgets[0] has the focus, find out what group it's in
	/// and focus the first widget in the next group.
	/// 
	/// If the focused widget was in the last group, wrap to focus the first widget
	/// of the first group
	///
	/// If nothing in this->widgets is focused, widgets[0] will be selected.
	/// </summary>
	/// <remarks>
	/// For details on how the borders of UI groups are defined, see the declaration
	/// and usage of enum CustomUIFlag::IsGroupStart.
	/// </remarks>
	/// <param name="uiSys">
	/// The UISystem that owns the widgets - this is needed because it has the
	/// selection querying/modification utilities.</param>
	void JumpSelectionToNextGroup(UISys& uiSys);

	/// <summary>
	/// Advance focus to the next widget in the group. i.e.:
	/// 
	/// If something in widgets[0] has the focus, advance the focus to select the
	/// next widget (ideally the next widget in the same group).
	/// 
	/// If the next widget is not in the same group, wrap to select the first widget
	/// in the group.
	/// 
	/// If nothing in this->widgets is focused, widgets[0] will be selected.
	/// </summary>
	/// <remarks>
	/// For details on how the borders of UI groups are defined, see the declaration
	/// and usage of enum CustomUIFlag::IsGroupStart.
	/// </remarks>
	/// The UISystem that owns the widgets - this is needed because it has the
	/// selection querying/modification utilities.</param>
	void MoveSelectionToNextChildInGroup(UISys& uiSys);
};