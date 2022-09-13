#pragma once
#include "../Substate.h"
#include "../StateHMDOp.h"
#include "../MousepadUI.h"

/// <summary>
/// The base substate type for input handlers used in StateHMDOp.
/// 
/// The class also forces these substate to provide information
/// for what the various mouse button inputs do, as well as their
/// icons used.
/// 
/// The icons loaded from GetIconPath() will be managed by an
/// asset manager outside of this system (most likely owned by
/// StateHMDOp).
/// </summary>
class HMDOpSub_Base : 
	public Substate<StateHMDOp>,
	public IMousepadUIBehaviour
{

protected:
	// We're going to cache our callback elements, because they should
	// be valid if we store them up-front, but also because we're going
	// to be calling our own input event handlers at arbitrary times when
	// these values won't be readily available.
	StateHMDOp* cachedOwner;
	SubstateMachine<StateHMDOp>* cachedSubStateMachine;

public:
	HMDOpSub_Base(StateHMDOp* owner, SubstateMachine<StateHMDOp>* substateMachine);
	void OnLeftUpHold( StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnRightUpHold(	StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;

	void HandleMessage(const Message& msg) override;
};