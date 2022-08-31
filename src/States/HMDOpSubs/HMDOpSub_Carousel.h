#pragma once

#include "HMDOpSub_Base.h"

/// <summary>
/// UI Substate to manage input in the context of when
/// the StateHMDOp is showing the surgery stage carousel.
/// </summary>
class HMDOpSub_Carousel : public HMDOpSub_Base
{
public:
	HMDOpSub_Carousel();
	void OnLeftDown(		StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnLeftUp(			StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnMiddleUp(		StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnMiddleUpHold(	StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnRightDown(		StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnRightUp(			StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnEnterContext(	StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnExitContext(		StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	std::string GetStateName() const override;

	std::string GetIconPath(ButtonID bid, StateHMDOp& targ) override;
	std::string GetActionName(ButtonID bid, StateHMDOp& targ) override;
	bool GetButtonUsable(ButtonID bid, StateHMDOp& targ) override;
};