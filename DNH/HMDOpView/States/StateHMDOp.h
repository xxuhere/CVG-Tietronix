#pragma once
#include "BaseState.h"
#include "../FontMgr.h"
#include "../CamVideo/CamStreamMgr.h"
#include "../TexObj.h"
#include "../Utils/cvgRect.h"
#include "../Utils/cvgCamTextureRegistry.h"

/// <summary>
/// The application state for the main operator loop.
/// </summary>
class StateHMDOp : public BaseState
{
public:
	bool inspectorShow = false;
	float maxInspectorWidth = 200.0f;

	TexObj ico_MenuAlign ;
	TexObj ico_MenuSliders;
	TexObj ico_MenuLaser;
	TexObj ico_MenuReturn;

	cvgCamTextureRegistry camTextureRegistry;

	FontWU fontInsTitle;

public:
	StateHMDOp(HMDOpApp* app, GLWin* view, MainWin* core);

	void DrawMenuSystemAroundRect(const cvgRect& rectDrawAround);

public:
	void Draw(const wxSize& sz) override;
	void Update(double dt) override;

	void EnteredActive() override;
	void ExitedActive() override;

	void OnKeydown(wxKeyCode key) override;

	void Initialize() override;
	void ClosingApp() override;

	~StateHMDOp();
};