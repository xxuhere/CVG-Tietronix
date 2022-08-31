#include "HMDOpSub_Default.h"
#include "../StateHMDOp.h"
#include "../../MainWin.h"
#include "../AppCoroutines/CoroutineSnapWithLasers.h"

HMDOpSub_Default::HMDOpSub_Default()
{}

void HMDOpSub_Default::OnLeftDown(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	targ.GetView()->ResetSnapCoroutine(
		new CoroutineSnapWithLasers(
			targ.GetCoreWindow(),
			targ.GetView(),
			targ.GetSurgeryPhaseLabel()));
}

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
{
	// If nothing is shown an the right pedal is pressed, start video recording
	// of the composite stream
	if(targ.GetCoreWindow()->IsRecording(SpecialCams::Composite))
		targ.GetCoreWindow()->StopRecording(SpecialCams::Composite);
	else
		targ.GetCoreWindow()->RecordVideo(SpecialCams::Composite, "video");
}

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

std::string HMDOpSub_Default::GetIconPath(ButtonID bid, StateHMDOp& targ)
{
	switch(bid)
	{
	case ButtonID::Left:
		return "Assets/ButtonAnno/BAnno_Photo.png";

	case ButtonID::Middle:
		return "Assets/ButtonAnno/BAnno_Menu.png";

	case ButtonID::HoldMiddle:
		return "Assets/ButtonAnno/BAnno_Phase.png";

	case ButtonID::Right:
		{
			bool rec = IsRecording();
			return rec ? 
				"Assets/ButtonAnno/BAnno_StopVideo.png" : 
				"Assets/ButtonAnno/BAnno_StartVideo.png";
		}

	}
	return "";
}

std::string HMDOpSub_Default::GetActionName(ButtonID bid, StateHMDOp& targ)
{
	switch(bid)
	{
	case ButtonID::Left:
		return "Snap Photo";

	case ButtonID::Middle:
		return "Main Menu";

	case ButtonID::HoldMiddle:
		return "Surgery Phase";

	case ButtonID::Right:
		{
			
			bool isRec = IsRecording();
			return isRec ? "Stop Recording" : "Record Video";
		}
	}
	return "";
}

bool HMDOpSub_Default::GetButtonUsable(ButtonID bid, StateHMDOp& targ)
{
	switch(bid)
	{
	case ButtonID::Left:
		return true;

	case ButtonID::Middle:
		return true;

	case ButtonID::HoldMiddle:
		return true;

	case ButtonID::Right:
		return true;

	}
	return false;
}