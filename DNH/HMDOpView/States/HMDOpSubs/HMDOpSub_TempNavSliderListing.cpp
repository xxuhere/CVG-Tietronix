#include "HMDOpSub_TempNavSliderListing.h"
#include "HMDOpSub_InspNavForm.h"
#include "HMDOpSub_WidgetCtrl.h"
#include "../StateHMDOp.h"

HMDOpSub_TempNavSliderListing::HMDOpSub_TempNavSliderListing(UIBase* optBtn, UIBase* inspPlate)
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

void HMDOpSub_TempNavSliderListing::OnRightUp(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm) 
{
	UIBase* uiSel = targ.uiSys.GetSelected();
	if(uiSel == nullptr)
		return;

	if(targ.camButtonGrouping.find((UIButton*)uiSel) == targ.camButtonGrouping.end())
		return;

	if(targ.camButtonGrouping[(UIButton*)uiSel].size() == 0)
		return;

	this->entered = true;
	ssm.PushSubstate(new HMDOpSub_WidgetCtrl((UIButton*)uiSel));
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