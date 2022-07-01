#pragma once

#include "../Substate.h"
#include "../StateHMDOp.h"

/// <summary>
/// UI Substate to manage input in the context of when
/// the StateHMDOp is showing the surgery stage carousel.
/// </summary>
class HMDOpSub_Carousel : public Substate<StateHMDOp>
{
public:
	HMDOpSub_Carousel();
	void OnLeftDown(		StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnLeftUp(			StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnMiddleUp(		StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnMiddleUpHold(	StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnRightDown(		StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnEnterContext(	StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnExitContext(		StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	std::string GetStateName() const override;
};