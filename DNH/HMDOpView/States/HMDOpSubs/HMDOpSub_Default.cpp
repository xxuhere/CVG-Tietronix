#include "HMDOpSub_Default.h"
#include "../StateHMDOp.h"
#include "../../MainWin.h"

HMDOpSub_Default::HMDOpSub_Default()
{}

void HMDOpSub_Default::OnLeftDown(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	targ.GetCoreWindow()->RequestSnapAll(targ.carousel.GetCurrentLabel());
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
