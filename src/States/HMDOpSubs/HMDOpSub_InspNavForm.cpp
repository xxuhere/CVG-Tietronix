#include "../StateHMDOp.h"
#include "../../UISys/UISys.h"
#include "HMDOpSub_InspNavForm.h"

void HMDOpSub_InspNavForm::CollectSelectable( UIBase* root, std::vector<UIBase*>& vecw)
{
	std::vector<UIBase*> todo;
	todo.push_back(root);

	// Collect selectables in the entire heirarchy.
	while(!todo.empty())
	{
		UIBase* t = todo.back();
		todo.pop_back();

		if(t->IsSelectable())
			vecw.push_back(t);

		for(int i = t->ChildCt() - 1; i >= 0; --i)
			todo.push_back(t->GetChild(i));
	}
}

HMDOpSub_InspNavForm::HMDOpSub_InspNavForm(
	StateHMDOp* targ,
	SubstateMachine<StateHMDOp>* substateMachine, 
	UIBase* optButton, 
	UIBase* inspPlate)
	: HMDOpSub_Base(targ, substateMachine)
{
	this->inspectorPlate = inspPlate;
	this->optButton = optButton;

	CollectSelectable(inspPlate, this->widgets);
}

void HMDOpSub_InspNavForm::JumpSelectionToNextGroup(UISys& uiSys)
{
	UIBase* uiSel = uiSys.GetSelected();
	if(uiSel == nullptr)
	{ 
		uiSys.Select(this->widgets[0]);
		return;
	}

	auto itFind = std::find(this->widgets.begin(), this->widgets.end(), uiSel);
	if(itFind == this->widgets.end())
	{
		uiSys.Select(this->widgets[0]);
		return;
	}

	++itFind;
	for( ; itFind != this->widgets.end(); ++itFind)
	{
		if(!(*itFind)->HasCustomFlags(CustomUIFlag::IsGroupStart))
			continue;

		uiSys.Select(*itFind);
		return;
	}

	uiSys.Select(this->widgets[0]);
}

void HMDOpSub_InspNavForm::MoveSelectionToNextChildInGroup(UISys& uiSys)
{
	UIBase* uiSel = uiSys.GetSelected();
	if(uiSel == nullptr)
	{ 
		uiSys.Select(this->widgets[0]);
		return;
	}

	auto itFind = std::find(this->widgets.begin(), this->widgets.end(), uiSel);
	if(itFind == this->widgets.end())
	{
		uiSys.Select(this->widgets[0]);
		return;
	}

	auto itNext = itFind;
	++itNext;
	if(itNext != this->widgets.end() && !(*itNext)->HasCustomFlags(CustomUIFlag::IsGroupStart))
	{
		uiSys.Select(*itNext);
		return;
	}
	while(itFind != this->widgets.begin() && !(*itFind)->HasCustomFlags(CustomUIFlag::IsGroupStart))
	{
		--itFind;
	}
	uiSys.Select(*itFind);
}

void HMDOpSub_InspNavForm::OnLeftUp(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	// Cycle group
	this->MoveSelectionToNextChildInGroup(targ.uiSys);
}

void HMDOpSub_InspNavForm::OnMiddleUp(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	// MenuOpt
	this->JumpSelectionToNextGroup(targ.uiSys);
}

void HMDOpSub_InspNavForm::OnMiddleUpHold(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	ssm.PopSubstate();
}

void HMDOpSub_InspNavForm::OnRightUp(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	UIBase* sel = targ.uiSys.GetSelected();
	if(sel != nullptr)
		targ.uiSys.SubmitClick(sel, 2, UIVec2(), false);
}

void HMDOpSub_InspNavForm::OnEnterContext(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	if(!this->widgets.empty())
		targ.uiSys.Select(this->widgets[0]);
}

void HMDOpSub_InspNavForm::OnExitContext(StateHMDOp& targ, SubstateMachine<StateHMDOp>& ssm)
{
	if(this->optButton != nullptr)
		targ.uiSys.Select(this->optButton);
	//this->inspectorPlate->Hide();
}

std::string HMDOpSub_InspNavForm::GetStateName() const
{
	return "FormNav";
}

std::string HMDOpSub_InspNavForm::GetIconPath(ButtonID bid, bool isHold)
{
	switch(bid)
	{
	case ButtonID::Left:
		if(!isHold)
			return "Assets/ButtonAnno/BAnno_CycleNext.png";
		break;

	case ButtonID::Middle:
		if(!isHold)
			return "Assets/ButtonAnno/BAnno_CycleGroup.png";
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

std::string HMDOpSub_InspNavForm::GetActionName(ButtonID bid, bool isHold)
{
	switch(bid)
	{
	case ButtonID::Left:
		if(!isHold)
			return "Next Item";
		break;

	case ButtonID::Middle:
		if(!isHold)
			return "Next Group";
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

bool HMDOpSub_InspNavForm::GetButtonUsable(ButtonID bid, bool isHold)
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
			return true;
		break;

	}
	return this->HMDOpSub_Base::GetButtonUsable(bid, isHold);
}