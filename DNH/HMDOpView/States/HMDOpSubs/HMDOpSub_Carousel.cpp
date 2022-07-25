#include "HMDOpSub_Carousel.h"
#include "../StateHMDOp.h"

HMDOpSub_Carousel::HMDOpSub_Carousel()
{}

void HMDOpSub_Carousel::OnLeftDown(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	targ.MoveSurgeryPhaseLeft();
}

void HMDOpSub_Carousel::OnLeftUp(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) 
{}

void HMDOpSub_Carousel::OnMiddleUp(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	ssm.PopSubstate();
}

void HMDOpSub_Carousel::OnMiddleUpHold(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	ssm.PopSubstate();
}

void HMDOpSub_Carousel::OnRightDown(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	targ.MoveSurgeryPhaseRight();
}

void HMDOpSub_Carousel::OnEnterContext(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	targ.showMainMenu = false;
	targ.ShowSurgeryPhase(true);
}

void HMDOpSub_Carousel::OnExitContext(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	targ.ShowSurgeryPhase(false);
}

std::string HMDOpSub_Carousel::GetStateName() const
{
	return "Carousel";
}

std::string HMDOpSub_Carousel::GetIconPath(ButtonID bid, StateHMDOp& targ)
{
	switch(bid)
	{
	case ButtonID::Left:
		return "Assets/ButtonAnno/BAnno_Left.png";

	case ButtonID::Middle:
		return "Assets/ButtonAnno/BAnno_Return.png";

	case ButtonID::HoldMiddle:
		return "Assets/ButtonAnno/BAnno_Return.png";

	case ButtonID::Right:
		return "Assets/ButtonAnno/BAnno_Right.png";

	}
	return "";
}

std::string HMDOpSub_Carousel::GetActionName(ButtonID bid, StateHMDOp& targ)
{
	switch(bid)
	{
	case ButtonID::Left:
		return "Left";

	case ButtonID::Middle:
		return "Back";

	case ButtonID::HoldMiddle:
		return "Back";

	case ButtonID::Right:
		return "Right";

	}
	return "";
}

bool HMDOpSub_Carousel::GetButtonUsable(ButtonID bid, StateHMDOp& targ)
{
	switch(bid)
	{
	case ButtonID::Left:
		return targ.SurgeryPhase_AnyMoreOnLeft();

	case ButtonID::Middle:
		return true;

	case ButtonID::HoldMiddle:
		return true;

	case ButtonID::Right:
		return targ.SurgeryPhase_AnyMoreOnRight();

	}
	return false;
}