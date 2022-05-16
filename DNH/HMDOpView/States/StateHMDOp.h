#pragma once
#include "BaseState.h"
#include "../FontMgr.h"
#include "../CamVideo/CamStreamMgr.h"
#include "../TexObj.h"
#include "../Utils/cvgRect.h"
#include "../Utils/cvgCamTextureRegistry.h"
#include "../UISys/UISys.h"
#include "../UISys/UIPlate.h"

/// <summary>
/// The application state for the main operator loop.
/// </summary>
class StateHMDOp : public BaseState
{
public:
	struct MouseDownState
	{
		const double clickDecayRate = 1.0f;
	public:
		float sinceClick = 0.0f;
		bool isDown = false;

	public:
		void Reset();
		void Decay(double dt);
		void FlagUp();
		void FlagDown();
	};

public:
	bool inspectorShow = false;
	float maxInspectorWidth = 200.0f;

	UISys uiSys;

	const float maxVertWidth = 120.0f;
	const float minVertWidth = 40.0f;
	const float vertTransSpeed = 4.0f;
	float curVertWidth = minVertWidth;

	bool showVertMenu = false;


	TexObj ico_MenuAlign ;
	TexObj ico_MenuSliders;
	TexObj ico_MenuLaser;
	TexObj ico_MenuReturn;

	UIPlate* vertMenuPlate = nullptr;

	TexObj ico_MousePadLeft;
	TexObj ico_MousePadRight;
	TexObj ico_MousePadCrevice;
	TexObj ico_MousePadBall;

	cvgCamTextureRegistry camTextureRegistry;

	FontWU fontInsTitle;
	//
	MouseDownState mdsLeft;
	MouseDownState mdsRight;
	MouseDownState mdsMiddle;

public:
	StateHMDOp(HMDOpApp* app, GLWin* view, MainWin* core);

	void DrawMenuSystemAroundRect(const cvgRect& rectDrawAround);

public:
	void DrawMousePad(float x, float y, float scale, bool ldown, bool rdown, bool mdown);

	void Draw(const wxSize& sz) override;
	void Update(double dt) override;
	//
	void EnteredActive() override;
	void ExitedActive() override;
	//
	void OnKeydown(wxKeyCode key) override;
	void OnMouseDown(int button, const wxPoint& pt) override;
	void OnMouseUp(int button, const wxPoint& pt) override;
	//
	void Initialize() override;
	void ClosingApp() override;


	~StateHMDOp();
};