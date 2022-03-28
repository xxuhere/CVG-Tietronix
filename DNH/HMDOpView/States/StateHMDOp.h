#pragma once
#include "BaseState.h"


class StateHMDOp : public BaseState
{
public:
	StateHMDOp(HMDOpApp* app, GLWin* view, MainWin* core);

	void Draw(const wxSize& sz) override;
	void Update() override;

	void EnteredActive() override;
	void ExitedActive() override;

	void Initialize() override;
	void ClosingApp() override;

	~StateHMDOp();
};