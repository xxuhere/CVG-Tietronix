#include "HMDOpSub_Carousel.h"
#include "../StateHMDOp.h"

HMDOpSub_Carousel::HMDOpSub_Carousel()
{}

void HMDOpSub_Carousel::OnLeftDown(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{}

void HMDOpSub_Carousel::OnLeftUp(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) 
{
	targ.SelectNextCarousel(false);
}

void HMDOpSub_Carousel::OnMiddleUp(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	targ.MoveSelectedCarouselRight(true);
}

void HMDOpSub_Carousel::OnMiddleUpHold(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	ssm.PopSubstate();
}

void HMDOpSub_Carousel::OnRightDown(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{}

void HMDOpSub_Carousel::OnRightUp( StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	targ.SelectNextCarousel(true);
}

void HMDOpSub_Carousel::OnEnterContext(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	targ.showMainMenu = false;
	targ.ShowCarousels(true);
}

void HMDOpSub_Carousel::OnExitContext(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	targ.ShowCarousels(false);
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
		return "Prev Carousel";

	case ButtonID::Middle:
		return "Next Entry";

	case ButtonID::HoldMiddle:
		return "Back";

	case ButtonID::Right:
		return "Next Carousel";

	}
	return "";
}

bool HMDOpSub_Carousel::GetButtonUsable(ButtonID bid, StateHMDOp& targ)
{
	switch(bid)
	{
	case ButtonID::Left:
		return true;
		//return targ.DoesCarouselHaveMoreOnLeft();

	case ButtonID::Middle:
		return true;

	case ButtonID::HoldMiddle:
		return true;

	case ButtonID::Right:
		return true;
		//return targ.DoesCarouselHaveMoreOnRight();

	}
	return false;
}