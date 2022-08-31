#include "HMDOpSub_Base.h"
#include "../AppCoroutines/CoroutineSnapWithLasers.h"

void HMDOpSub_Base::OnLeftUpHold( StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	targ.GetView()->ResetSnapCoroutine(
		new CoroutineSnapWithLasers(
			targ.GetCoreWindow(),
			targ.GetView(),
			targ.GetSurgeryPhaseLabel()));
}

void HMDOpSub_Base::OnRightUpHold( StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	// If nothing is shown an the right pedal is pressed, start video recording
	// of the composite stream
	if(targ.GetCoreWindow()->IsRecording(SpecialCams::Composite))
		targ.GetCoreWindow()->StopRecording(SpecialCams::Composite);
	else
		targ.GetCoreWindow()->RecordVideo(SpecialCams::Composite, "video");
}