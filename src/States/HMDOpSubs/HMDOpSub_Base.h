#pragma once
#include "../Substate.h"
#include "../StateHMDOp.h"

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
class HMDOpSub_Base : public Substate<StateHMDOp>
{
public:
	enum class ButtonID
	{
		Left = 0,
		Middle = 1,
		Right = 2,
		HoldMiddle = 3,
		Totalnum
	};

public:
	/// <summary>
	/// Get the path of the icon to load and use for the
	/// action's overlay of specific mouse pad buttons.
	/// </summary>
	/// <param name="bid">The button to provide an icon path for.</param>
	/// <param name="">The parent StateHMDOp.</param>
	/// <returns>The path, or "" to return nothing.</returns>
	virtual std::string GetIconPath(ButtonID bid, StateHMDOp& targ) = 0;

	/// <summary>
	/// Get the name of the action for a mouse pad button.
	/// </summary>
	/// <param name="bid">The button to get the name of.</param>
	/// /// <param name="">The parent StateHMDOp.</param>
	/// <returns>The queried name.</returns>
	virtual std::string GetActionName(ButtonID bid, StateHMDOp& targ) = 0;

	/// <summary>
	/// Specify if a button is active.
	/// </summary>
	/// <param name="bid">The button to query the usability status of.</param>
	/// <param name="">The parent StateHMDOp.</param>
	/// <returns>If true, button is usable, else the button does nothing.</returns>
	virtual bool GetButtonUsable(ButtonID bid, StateHMDOp& targ) = 0;

	void OnLeftUpHold( StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;
	void OnRightUpHold(	StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) override;


};