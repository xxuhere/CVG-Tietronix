#pragma once

#include "HMDOpSub_Base.h"

class StateHMDOp;

/// <summary>
/// UI Substate to manage input in the context of when
/// the StateHMDOp is showing an array of button selections that
/// redirect to manipulating sliders.
/// </summary>
class HMDOpSub_TempNavSliderListing : public HMDOpSub_Base
{
	/// <summary>
	/// The UI plate that is the parent to all the UI elements
	/// that can be navigated.
	/// </summary>
	UIBase* inspPlate;

	/// <summary>
	/// The main menu button that was selected to enter into this
	/// view. This is so when we exit the view, we can restore 
	/// the selection back.
	/// </summary>
	UIBase* optnBtn;

	/// <summary>
	/// The various widgets that can be navigated by clicking.
	/// 
	/// For information on how the system knows where button groups
	/// end, see CustomUIFlag::IsGroupStart.
	/// </summary>
	std::vector<UIBase*> widgets;

	/// <summary>
	/// Used to check if we're leaving the state because we're going
	/// into a deeper context, or poping.
	/// 
	/// Only when popping do we restore selection to optnBtn.
	/// </summary>
	bool entered = false;

public:

	HMDOpSub_TempNavSliderListing(StateHMDOp* targ, SubstateMachine<StateHMDOp>* substateMachine, UIBase* optBtn, UIBase* inspPlate);
	void OnLeftUp(			StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnMiddleUp(		StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnMiddleUpHold(	StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnRightUp(			StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnEnterContext(	StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnExitContext(		StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	std::string GetStateName() const override;

	/// <summary>
	/// Ensure something in the target menu is focused. And if something in
	/// that set is already focused, advance the focus to the next thing. 
	/// Also handles wrapping to the start when the last widget is advanced.
	/// </summary>
	/// <param name="targ">
	/// The StateHMDOp, for access to its UI information and UISys.
	/// </param>
	void MoveFocus(StateHMDOp& targ);

	/// <summary>
	/// Checks if the selected button has sliders.
	/// </summary>
	/// <returns>If false, nullptr. If true, the selected button.</returns>
	UIBase* SelectedButtonHasSliders(StateHMDOp& targ);

	std::string GetIconPath(ButtonID bid, bool isHold) override;
	std::string GetActionName(ButtonID bid, bool isHold) override;
	bool GetButtonUsable(ButtonID bid, bool isHold) override;
};