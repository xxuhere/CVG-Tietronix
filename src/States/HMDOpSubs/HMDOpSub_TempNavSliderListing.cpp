#include "HMDOpSub_TempNavSliderListing.h"
#include "HMDOpSub_InspNavForm.h"
#include "HMDOpSub_WidgetCtrl.h"
#include "../StateHMDOp.h"

HMDOpSub_TempNavSliderListing::HMDOpSub_TempNavSliderListing(
	StateHMDOp* owner, 
	SubstateMachine<StateHMDOp>* substateMachine, 
	UIBase* optBtn, 
	UIBase* inspPlate)
	: HMDOpSub_Base(owner, substateMachine)
{
	this->optnBtn = optBtn;
	this->inspPlate = inspPlate;

	HMDOpSub_InspNavForm::CollectSelectable(inspPlate, this->widgets);
}

void HMDOpSub_TempNavSliderListing::MoveFocus(StateHMDOp& targ)
{
	if(this->widgets.empty())
		return;

	UIBase* uiSel = targ.uiSys.GetSelected();
	if(uiSel == nullptr)
	{
		targ.uiSys.Select(this->widgets[0]);
		return;
	}

	auto itFind = std::find(this->widgets.begin(), this->widgets.end(), uiSel);
	if(itFind == this->widgets.end())
	{
		targ.uiSys.Select(this->widgets[0]);
		return;
	}

	++itFind;
	if(itFind != this->widgets.end())
		targ.uiSys.Select(*itFind);
	else
		targ.uiSys.Select(this->widgets[0]);
}

void HMDOpSub_TempNavSliderListing::OnLeftUp(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) 
{
	this->MoveFocus(targ);

	UIBase* uiSel = targ.uiSys.GetSelected();
	targ.ManageCamButtonPressed(uiSel->Idx());
}

void HMDOpSub_TempNavSliderListing::OnMiddleUp(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	this->MoveFocus(targ);

	UIBase* uiSel = targ.uiSys.GetSelected();
	targ.ManageCamButtonPressed(uiSel->Idx());

}

void HMDOpSub_TempNavSliderListing::OnMiddleUpHold(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	ssm.PopSubstate();
}

UIBase* HMDOpSub_TempNavSliderListing::SelectedButtonHasSliders(StateHMDOp& targ)
{
	UIBase* uiSel = targ.uiSys.GetSelected();
	if(uiSel == nullptr)
		return nullptr;

	if(targ.camButtonGrouping.find((UIButton*)uiSel) == targ.camButtonGrouping.end())
		return nullptr;

	if(targ.camButtonGrouping[(UIButton*)uiSel].size() == 0)
		return nullptr;

	return uiSel;
}

void HMDOpSub_TempNavSliderListing::OnRightUp(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) 
{
	UIBase* uiSel = SelectedButtonHasSliders(targ);
	if(uiSel == nullptr)
		return;

	this->entered = true;
	ssm.PushSubstate(new HMDOpSub_WidgetCtrl(this->cachedOwner, this->cachedSubStateMachine, (UIButton*)uiSel));
}

void HMDOpSub_TempNavSliderListing::OnEnterContext(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) 
{
	if(!this->widgets.empty())
	{
		UIBase* uiSel = targ.uiSys.GetSelected();

		// If a button is already selected, don't force the selection.
		//
		// But if nothing or and unknown thing is selected, for a 
		// default selection.
		if(std::find(this->widgets.begin(), this->widgets.end(), uiSel) == this->widgets.end())
			targ.uiSys.Select(this->widgets[0]);

		targ.ManageCamButtonPressed(uiSel->Idx());
	}
}

void HMDOpSub_TempNavSliderListing::OnExitContext(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) 
{
	if(this->optnBtn != nullptr)
		targ.uiSys.Select(this->optnBtn);

	if(this->entered)
	{ 
		// If we're exiting because we pushed a UI context, because what we have selected.
		this->entered = false;
	}
	else
	{
		// Else if we popped, get rid of what's selected.
		targ.ManageCamButtonPressed(-1);
	}
}

std::string HMDOpSub_TempNavSliderListing::GetStateName() const
{
	return "SliderListing";
}

std::string HMDOpSub_TempNavSliderListing::GetIconPath(ButtonID bid, bool isHold)
{
	switch(bid)
	{
	case ButtonID::Left:
		return "Assets/ButtonAnno/BAnno_CycleNext.png";
		break;

	case ButtonID::Middle:
		if(!isHold)
			return "Assets/ButtonAnno/BAnno_CycleNext.png";
		else
			return "Assets/ButtonAnno/BAnno_Return.png";
		break;

	case ButtonID::Right:
		return "Assets/ButtonAnno/BAnno_Toggle.png";
		break;

	}
	return "";
}

std::string HMDOpSub_TempNavSliderListing::GetActionName(ButtonID bid, bool isHold)
{
	switch(bid)
	{
	case ButtonID::Left:
		if(!isHold)
			return "Next Control";
		break;

	case ButtonID::Middle:
		if(!isHold)
			return "Next Control";
		else
			return "Go Back";
		break;

	case ButtonID::Right:
		if(!isHold)
			return "Select";
		break;

	}
	return this->HMDOpSub_Base::GetActionName(bid, isHold);
}

bool HMDOpSub_TempNavSliderListing::GetButtonUsable(ButtonID bid, bool isHold)
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
		break;

	case ButtonID::Right:
		if(!isHold)
			return this->SelectedButtonHasSliders(*this->cachedOwner);
		break;

	}
	return this->HMDOpSub_Base::GetButtonUsable(bid, isHold);
}