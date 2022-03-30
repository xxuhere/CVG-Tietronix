#pragma once
#include "BaseState.h"
#include "../FontMgr.h"
#include "../CamStreamMgr.h"
#include "../TexObj.h"
#include "../Utils/cvgRect.h"

/// <summary>
/// The application state for the main operator loop.
/// </summary>
class StateHMDOp : public BaseState
{
public:
	TexObj camFrame;
	long long lastFrameSeen = -1;

	bool inspectorShow = false;
	float maxInspectorWidth = 200.0f;

	TexObj ico_MenuAlign;
	TexObj ico_MenuSliders;
	TexObj ico_MenuLaser;
	TexObj ico_MenuReturn;

	FontWU fontInsTitle;

public:
	StateHMDOp(HMDOpApp* app, GLWin* view, MainWin* core);

	void DrawMenuSystemAroundRect(const cvgRect& rectDrawAround);

public:
	void Draw(const wxSize& sz) override;
	void Update(double dt) override;

	void EnteredActive() override;
	void ExitedActive() override;

	void Initialize() override;
	void ClosingApp() override;

	~StateHMDOp();
};