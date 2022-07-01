#include "HMDOpSub_MainMenuNav.h"
#include "HMDOpSub_TempNavSliderListing.h"
#include "HMDOpSub_InspNavForm.h"
#include "../StateHMDOp.h"
#include "../../UISys/UIButton.h"

HMDOpSub_MainMenuNav::HMDOpSub_MainMenuNav()
{}

void HMDOpSub_MainMenuNav::OnLeftDown(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	this->EnforceTabOrder(targ);
	targ.uiSys.AdvanceTabbingOrder(true);

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

void HMDOpSub_MainMenuNav::OnLeftUp(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{}

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
					targ.btnSettings, 
					targ.inspSettingsPlate));
			break;

		case StateHMDOp::UIID::MBtnSource:
			if(targ.inspSetFrame->IsSelfVisible())
				targ.SetShownMenuBarUIPanel(StateHMDOp::UIID::MBtnSource);

			ssm.PushSubstate(
				new HMDOpSub_TempNavSliderListing(
					targ.btnCamSets, 
					targ.inspCamSetsPlate));
			break;

		case StateHMDOp::UIID::MBtnBack:
			// TODO: This is broken if not using whiffs
			ssm.PopSubstate();
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
			targ.btnBack
		};
	}

	this->EnforceTabOrder(targ);
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