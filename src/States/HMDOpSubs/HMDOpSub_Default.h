#pragma once

#include "HMDOpSub_Base.h"


class StateHMDOp;

/// <summary>
/// UI Substate to manage input in the context of when
/// the StateHMDOp is showing the default view (when no
/// menus are shown).
/// </summary>
class HMDOpSub_Default : public HMDOpSub_Base
{
public:
	HMDOpSub_Default(StateHMDOp* owner, SubstateMachine<StateHMDOp>* substateMachine);
	void OnLeftDown(		StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnLeftUp(			StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnMiddleUp(		StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnMiddleUpHold(	StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnRightDown(		StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnRightUp(			StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnEnterContext(	StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnExitContext(		StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	std::string GetStateName() const override;

	std::string GetIconPath(ButtonID bid) override;
	std::string GetActionName(ButtonID bid) override;
	bool GetButtonUsable(ButtonID bid) override;
};