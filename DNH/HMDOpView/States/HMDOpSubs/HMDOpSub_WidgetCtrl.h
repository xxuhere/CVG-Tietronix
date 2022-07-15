#pragma once

#include "HMDOpSub_Base.h"

#include <vector>

class StateHMDOp;
class UIHSlider;
class UIButton;

/// <summary>
/// UI Substate to manage input in the context of when
/// the StateHMDOp is in a view that shows sliders and allows
/// manipulating them with button presses.
/// </summary>
class HMDOpSub_WidgetCtrl : public HMDOpSub_Base
{
public:
	/// <summary>
	/// The button that activated the view. This is so when the 
	/// state is popped, we can restore focus to it.
	/// </summary>
	UIButton* categoryBtn;

	/// <summary>
	/// The various sliders in the context that can be navigated
	/// by middle clicking.
	/// </summary>
	std::vector<UIHSlider*> sliders;

public:
	HMDOpSub_WidgetCtrl(UIButton* btn);
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
};