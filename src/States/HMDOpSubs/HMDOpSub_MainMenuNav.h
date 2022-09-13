#pragma once

#include <vector>

#include "HMDOpSub_Base.h"

class StateHMDOp;

/// <summary>
/// UI Substate to manage input in the context of when
/// the StateHMDOp is showing the main menu.
/// </summary>
class HMDOpSub_MainMenuNav : public HMDOpSub_Base
{
public:
	/// <summary>
	/// A cache of the buttons in the main menu.
	/// </summary>
	std::vector<UIBase*> menuButtons;

public:
	HMDOpSub_MainMenuNav(StateHMDOp* owner, SubstateMachine<StateHMDOp>* substateMachine);
	void OnLeftDown(		StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnLeftUp(			StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnMiddleUp(		StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnMiddleUpHold(	StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnRightUp(			StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnEnterContext(	StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnExitContext(		StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	std::string GetStateName() const override;

	/// <summary>
	/// Set the custom tab order to only be limited to the main
	/// menu widgets, and set the selection to the first widget
	/// in the main menu if nothing in the main menu is selected.
	/// </summary>
	/// <param name="targ">
	/// The StateHMDOp for access to the UI elements and UISys.
	/// </param>
	void EnforceTabOrder(StateHMDOp& targ);

	std::string GetIconPath(ButtonID bid, bool isHold) override;
	std::string GetActionName(ButtonID bid, bool isHold) override;
	bool GetButtonUsable(ButtonID bid, bool isHold) override;

	/// <summary>
	/// Should be called whenever the main menu select state has changed
	/// and the shown submenu needs to be updated.
	/// </summary>
	/// <param name="targ">The HMDOp with the state and UI data.</param>
	void UpdateSelectedSubmenu(StateHMDOp& targ);
};