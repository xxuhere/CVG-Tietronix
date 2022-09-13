#include "HMDOpSub_Carousel.h"
#include "../StateHMDOp.h"

HMDOpSub_Carousel::HMDOpSub_Carousel(
	StateHMDOp* owner, 
	SubstateMachine<StateHMDOp>* substateMachine)
	: HMDOpSub_Base(owner, substateMachine)
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

std::string HMDOpSub_Carousel::GetIconPath(ButtonID bid, bool isHold)
{
	switch(bid)
	{
	case ButtonID::Left:
		if(!isHold)
			return "Assets/ButtonAnno/BAnno_Left.png";
		break;

	case ButtonID::Middle:
		return !isHold ?
			"Assets/ButtonAnno/BAnno_CycleNext.png" :
			"Assets/ButtonAnno/BAnno_Return.png";

	case ButtonID::Right:
		if(!isHold)
			return "Assets/ButtonAnno/BAnno_Right.png";
		break;

	}
	return this->HMDOpSub_Base::GetIconPath(bid, isHold);
}

std::string HMDOpSub_Carousel::GetActionName(ButtonID bid, bool isHold)
{
	switch(bid)
	{
	case ButtonID::Left:
		if(!isHold)
			return "Prev Carousel";
		break;

	case ButtonID::Middle:
		return !isHold ? 
			"Next Entry" :
			"Back";

	case ButtonID::Right:
		if(!isHold)
			return "Next Carousel";
		break;

	}
	return this->HMDOpSub_Base::GetActionName(bid, isHold);
}

bool HMDOpSub_Carousel::GetButtonUsable(ButtonID bid, bool isHold)
{
	switch(bid)
	{
	case ButtonID::Left:
		if(!isHold)
			return true;
		break;

	case ButtonID::Middle:
		if(!isHold)
			return true;
		else
			return true;

	case ButtonID::Right:
		if(!isHold)
			return true;
		break;

	}
	return this->HMDOpSub_Base::GetButtonUsable(bid, isHold);
}