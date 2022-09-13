#include "HMDOpSub_Default.h"
#include "../StateHMDOp.h"
#include "../../MainWin.h"

HMDOpSub_Default::HMDOpSub_Default(
	StateHMDOp* targ, 
	SubstateMachine<StateHMDOp>* substateMachine)
	: HMDOpSub_Base(targ, substateMachine)
{}

void HMDOpSub_Default::OnLeftDown(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{}

void HMDOpSub_Default::OnLeftUp(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
}

void HMDOpSub_Default::OnMiddleUp(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	ssm.PushCachedSubstate((int)CoreSubState::MenuNav);
}

void HMDOpSub_Default::OnMiddleUpHold(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	ssm.PushCachedSubstate((int)CoreSubState::CarouselStage);
}

void HMDOpSub_Default::OnRightDown(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{}

void HMDOpSub_Default::OnRightUp(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{}

void HMDOpSub_Default::OnEnterContext(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	targ.CloseShownMenuBarUIPanel();
	targ.showMainMenu = false;
}

void HMDOpSub_Default::OnExitContext(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{}

std::string HMDOpSub_Default::GetStateName() const
{
	return "Default";
}

bool IsRecording()
{
	CamStreamMgr& camMgr = CamStreamMgr::GetInstance();
	return camMgr.IsRecording(SpecialCams::Composite);
}

std::string HMDOpSub_Default::GetIconPath(ButtonID bid, bool isHold)
{
	switch(bid)
	{
	case ButtonID::Left:
		if(!isHold)
			return "";
		break;

	case ButtonID::Middle:
		if(!isHold)
			return "Assets/ButtonAnno/BAnno_Menu.png";
		else
			return "Assets/ButtonAnno/BAnno_Phase.png";

	case ButtonID::Right:
		{
			if(!isHold)
			{ 
				bool rec = IsRecording();
				return rec ? 
					"Assets/ButtonAnno/BAnno_StopVideo.png" : 
					"Assets/ButtonAnno/BAnno_StartVideo.png";
			}
		}
		break;

	}
	return this->HMDOpSub_Base::GetIconPath(bid, isHold);
}

std::string HMDOpSub_Default::GetActionName(ButtonID bid, bool isHold)
{
	switch(bid)
	{
	case ButtonID::Left:
		if(!isHold)
			return "";
		break;

	case ButtonID::Middle:
		if(!isHold)
			return "Main Menu";
		else
			return "Surgery Phase";
		break;

	case ButtonID::Right:
		if(!isHold)
			return "";
		break;
	}
	return this->HMDOpSub_Base::GetActionName(bid, isHold);
}

bool HMDOpSub_Default::GetButtonUsable(ButtonID bid, bool isHold)
{
	switch(bid)
	{
	case ButtonID::Left:
		if(!isHold)
			return false;
		break;

	case ButtonID::Middle:
		// Yes, this can be optimized, but we separate it out to show
		// our explicit intent of handling both.
		if(!isHold) 
			return true;
		else
			return true;

	case ButtonID::Right:
		if(!isHold)
			return false;
		break;

	}
	return this->HMDOpSub_Base::GetButtonUsable(bid, isHold);
}