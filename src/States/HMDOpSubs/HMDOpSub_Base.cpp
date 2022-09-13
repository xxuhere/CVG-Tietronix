#include "HMDOpSub_Base.h"
#include "../AppCoroutines/CoroutineSnapWithLasers.h"

HMDOpSub_Base::HMDOpSub_Base(StateHMDOp* targ, SubstateMachine<StateHMDOp>* substateMachine)
{
	this->cachedTarget = targ;
	this->cachedOwner = substateMachine;
}

void HMDOpSub_Base::OnLeftUpHold( StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	targ.GetView()->ResetSnapCoroutine(
		new CoroutineSnapWithLasers(
			targ.GetCoreWindow(),
			targ.GetView(),
			targ.GetSurgerySeriesLabel()));
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

std::string HMDOpSub_Base::GetIconPath(ButtonID bid, bool isHold)
{
	if(isHold)
	{ 
		switch(bid)
		{
		case ButtonID::Left:
			return "Assets/ButtonAnno/BAnno_Photo.png";

		case ButtonID::Right:
			if(this->cachedTarget->GetCoreWindow()->IsRecording(SpecialCams::Composite))
				return "Assets/ButtonAnno/BAnno_StopVideo.png";
			else
				return "Assets/ButtonAnno/BAnno_StartVideo.png";
				
		}
	}

	// Besides hold left and hold right, subclasses are expected to (ATM)
	// handle everything else.
	std::cout << "WARNING: Unexpected unhandled GetIconPath()" << std::endl;
	return "";
}

std::string HMDOpSub_Base::GetActionName(ButtonID bid, bool isHold)
{
	if(isHold)
	{
		switch(bid)
		{
		case ButtonID::Left:
			return "Snapshots";

		case ButtonID::Right:
			if(this->cachedTarget->GetCoreWindow()->IsRecording(SpecialCams::Composite))
				return "Stop Recording";
			else
				return "Start Recording";
			break;
		}
	}

	// Besides hold left and hold right, subclasses are expected to (ATM)
	// handle everything else.
	std::cout << "WARNING: Unexpected unhandled GetActionName()" << std::endl;
	return "";
}

bool HMDOpSub_Base::GetButtonUsable(ButtonID bid, bool isHold)
{
	if(isHold)
	{
		switch(bid)
		{
		case ButtonID::Left:
			return true;
			break;

		case ButtonID::Right:
			return true;
			break;
		}
	}

	// Besides hold left and hold right, subclasses are expected to (ATM)
	// handle everything else.
	std::cout << "WARNING: Unexpected unhandled GetButtonUsable()" << std::endl;
	return false;
}

void HMDOpSub_Base::HandleMessage(const Message& msg)
{
	// For the base implementation, redirect the plumming to the functions
	// already provided in the Substate we derive off of, and just make sure
	// to disconnect it from StateHMDOp's event handler.

	switch(msg.msgTy)
	{
	case MessageType::Down:
		switch(msg.idx)
		{
		case 0:
			this->OnLeftDown(*this->cachedTarget, *this->cachedOwner);
			break;

		case 1:
			this->OnMiddleDown(*this->cachedTarget, *this->cachedOwner);
			break;

		case 2:
			this->OnRightDown(*this->cachedTarget, *this->cachedOwner);
			break;
		}
		break;

	case MessageType::HoldUp:
		switch(msg.idx)
		{
		case 0:
			this->OnLeftUpHold(*this->cachedTarget, *this->cachedOwner);
			break;

		case 1:
			this->OnMiddleUpHold(*this->cachedTarget, *this->cachedOwner);
			break;

		case 2:
			this->OnRightUpHold(*this->cachedTarget, *this->cachedOwner);
			break;
		}
		break;

	case MessageType::Up:
		switch(msg.idx)
		{
		case 0:
			this->OnLeftUp(*this->cachedTarget, *this->cachedOwner);
			break;

		case 1:
			this->OnMiddleUp(*this->cachedTarget, *this->cachedOwner);
			break;

		case 2:
			this->OnRightUp(*this->cachedTarget, *this->cachedOwner);
			break;
		}
		break;
	}
}