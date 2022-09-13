#include "HMDOpSub_MainMenuNav.h"
#include "HMDOpSub_TempNavSliderListing.h"
#include "HMDOpSub_InspNavForm.h"
#include "../StateHMDOp.h"
#include "../../UISys/UIButton.h"

HMDOpSub_MainMenuNav::HMDOpSub_MainMenuNav(
	StateHMDOp* owner, 
	SubstateMachine<StateHMDOp>* substateMachine)
	: HMDOpSub_Base(owner, substateMachine)
{}

void HMDOpSub_MainMenuNav::UpdateSelectedSubmenu(StateHMDOp& targ)
{
	UIBase* curSel = targ.uiSys.GetSelected();
	if(curSel->HasCustomFlags(CustomUIFlag::IsMainOptWithNoContents))
	{
		targ.CloseShownMenuBarUIPanel();
	}
	else
	{
		targ.SetShownMenuBarUIPanel(curSel->Idx());
	}
}

void HMDOpSub_MainMenuNav::OnLeftDown(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{}

void HMDOpSub_MainMenuNav::OnLeftUp(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	this->EnforceTabOrder(targ);
	targ.uiSys.AdvanceTabbingOrder(true);

	this->UpdateSelectedSubmenu(targ);
}

void HMDOpSub_MainMenuNav::OnMiddleUp(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{}

void HMDOpSub_MainMenuNav::OnMiddleUpHold(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	ssm.PopSubstate();
}

void HMDOpSub_MainMenuNav::OnRightUp(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	UIBase* uiSel = targ.uiSys.GetSelected();

	if(uiSel != nullptr)
	{ 
		switch(uiSel->Idx())
		{
		case StateHMDOp::UIID::MBtnLaserTog:
			targ.uiSys.SubmitClick(targ.btnLaser, 2, UIVec2(), false);
			break;

		case StateHMDOp::UIID::MBtnLaserSet:
			if(!targ.inspSettingsPlate->IsSelfVisible())
				targ.SetShownMenuBarUIPanel(StateHMDOp::UIID::MBtnLaserSet);

			ssm.PushSubstate(
				new HMDOpSub_InspNavForm(
					this->cachedOwner,
					&ssm,
					targ.btnSettings, 
					targ.inspSettingsPlate));
			break;

		case StateHMDOp::UIID::MBtnSource:
			if(targ.inspSetFrame->IsSelfVisible())
				targ.SetShownMenuBarUIPanel(StateHMDOp::UIID::MBtnSource);

			ssm.PushSubstate(
				new HMDOpSub_TempNavSliderListing(
					this->cachedOwner,
					&ssm,
					targ.btnCamSets, 
					targ.inspCamSetsPlate));
			break;


		case StateHMDOp::UIID::MBtnExit:
			ssm.PushSubstate(
				new HMDOpSub_InspNavForm(
					this->cachedOwner,
					&ssm,
					targ.btnExit, 
					targ.inspExitPlate));
			break;
		}
	}
}

void HMDOpSub_MainMenuNav::OnEnterContext(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	targ.showMainMenu = true;

	if(this->menuButtons.empty())
	{ 
		this->menuButtons = 
		{
			targ.btnLaser, 
			targ.btnSettings, 
			targ.btnAlign, 
			targ.btnCamSets, 
			targ.btnExit
		};
	}

	this->EnforceTabOrder(targ);
	this->UpdateSelectedSubmenu(targ);
}

void HMDOpSub_MainMenuNav::OnExitContext(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	targ.uiSys.ClearCustomTabOrder();
}

void HMDOpSub_MainMenuNav::EnforceTabOrder(StateHMDOp& targ)
{
	targ.uiSys.SetCustomTabOrder(menuButtons);

	UIBase* curSel = targ.uiSys.GetSelected();
	if(std::find(menuButtons.begin(), menuButtons.end(), curSel) == menuButtons.end())
		targ.uiSys.Select(menuButtons[0]);
}

std::string HMDOpSub_MainMenuNav::GetStateName() const
{
	return "MenuNav";
}

std::string HMDOpSub_MainMenuNav::GetIconPath(ButtonID bid, bool isHold)
{
	switch(bid)
	{
	case ButtonID::Left:
		if(!isHold)
			return "Assets/ButtonAnno/BAnno_CycleNext.png";
		break;

	case ButtonID::Middle:
		if(!isHold)
			return "";
		else
			return "Assets/ButtonAnno/BAnno_Return.png";
		break;

	case ButtonID::Right:
		if(!isHold)
			return "Assets/ButtonAnno/BAnno_Toggle.png";
		break;

	}
	return this->HMDOpSub_Base::GetIconPath(bid, isHold);
}

std::string HMDOpSub_MainMenuNav::GetActionName(ButtonID bid, bool isHold)
{
	switch(bid)
	{
	case ButtonID::Left:
		if(!isHold)
			return "Next Selection";
		break;

	case ButtonID::Middle:
		if(!isHold)
			return "--";
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

bool HMDOpSub_MainMenuNav::GetButtonUsable(ButtonID bid, bool isHold)
{
	switch(bid)
	{
	case ButtonID::Left:
		if(!isHold)
			return true;
		break;

	case ButtonID::Middle:
		if(!isHold)
			return false;
		else
			return true;
		break;

	case ButtonID::Right:
		if(!isHold)
		{
			// This subme
			UIBase* uibSel = this->cachedOwner->uiSys.GetSelected();
			if(uibSel == nullptr)
				return false;

			return uibSel->Idx() != StateHMDOp::MBtnAlign;
		}
		break;

	}
	return HMDOpSub_Base::GetButtonUsable(bid, isHold);
}