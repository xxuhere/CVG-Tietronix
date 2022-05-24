#pragma once
#include "BaseState.h"
#include "../FontMgr.h"
#include "../CamVideo/CamStreamMgr.h"
#include "../TexObj.h"
#include "../Utils/cvgRect.h"
#include "../Utils/cvgCamTextureRegistry.h"
#include "../UISys/UISys.h"
#include "../UISys/UIPlate.h"
#include "../UISys/UIText.h"
#include "../UISys/UIVBulkSlider.h"

class UIButton;

/// <summary>
/// The application state for the main operator loop.
/// </summary>
class StateHMDOp : 
	public BaseState,
	public UISink
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

	enum UIID
	{
		// Main Menubar
		PullMenu,
		MBtnLaserTog,
		MBtnLaserSet,
		MBtnAlign,
		MBtnSource,
		// Laser Menu
		LaseWat_1,
		LaseWat_2,
		LaseWat_3,
		Lase_Exposure_1,
		Lase_Exposure_2,
		Lase_Exposure_3,
		Lase_Exposure_4,
		Lase_ThresholdType,
		Lase_ThresholdToggle,
		// Camera Settings
		CamSet_Exposure,
		CamSet_Disparity,
		CamSet_Opacity,
		CamSet_Register,
		CamSet_Calibrate,
		CamSet_Threshold,
		CamSet_Threshold_SlideThresh,
		CamSet_Threshold_DispUp,
		CamSet_Threshold_Back,
		CamSet_Opacity_Meter,
		CamSet_Opacity_Back,
		CamSet_Calibrate_Slider,
	};

public:
	bool inspectorShow = false;
	float maxInspectorWidth = 200.0f;

	UISys uiSys;

	const float maxVertWidth = 120.0f;
	const float minVertWidth = 40.0f;
	const float vertTransSpeed = 400.0f;
	float curVertWidth = minVertWidth;


	bool showVertMenu = false;

	cvgCamTextureRegistry camTextureRegistry;

	FontWU fontInsTitle;

	UIRect lastViewportRect;

	//////////////////////////////////////////////////
	//
	//		UI Assets
	//
	//////////////////////////////////////////////////

	// These are for standard graphical elements. If there are
	// other states that need this, this needs to be moved out
	// of the class and somewhere more accessible to all states,
	// such as the GLWin.

	TexObj::SPtr patch_circle;
	TexObj::SPtr patch_roundLeft;
	TexObj::SPtr patch_roundRight;
	NinePatcher ninePatchCircle;

	TexObj::SPtr patch_smallCircle;
	NinePatcher ninePatchSmallCircle;

	//////////////////////////////////////////////////
	//
	//		MOUSE PAD GRAPHIC VARIABLES
	//
	//////////////////////////////////////////////////

	// Various icons for the mouse pad graphic
	TexObj ico_MousePadLeft;
	TexObj ico_MousePadRight;
	TexObj ico_MousePadCrevice;
	TexObj ico_MousePadBall;

	// Animation state information for the mouse
	// pad graphic buttons.
	MouseDownState mdsLeft;
	MouseDownState mdsRight;
	MouseDownState mdsMiddle;

	//////////////////////////////////////////////////
	//
	//		RIGHT MAIN MENU BAR
	//
	//////////////////////////////////////////////////
	UIPlate* vertMenuPlate		= nullptr;
	UIButton* btnLaser			= nullptr;
	UIButton* btnSettings		= nullptr;
	UIButton* btnAlign			= nullptr;
	UIButton* btnCamSets		= nullptr;

	//////////////////////////////////////////////////
	//
	//		LASER SETTINGS MENU
	//
	//////////////////////////////////////////////////
	UIPlate* inspSettingsPlate	= nullptr;
	UIPlate* inspSetFrame		= nullptr;
	UIButton* btnLaseW_1		= nullptr;
	UIButton* btnLaseW_2		= nullptr;
	UIButton* btnLaseW_3		= nullptr;
	UIButton* btnExp_1			= nullptr;
	UIButton* btnExp_2			= nullptr;
	UIButton* btnExp_3			= nullptr;
	UIButton* btnExp_4			= nullptr;
	UIButton* btnThreshTy		= nullptr;
	UIButton* btnThreshTog		= nullptr;

	UIPlate* plCamThreshold		= nullptr;
	UIPlate* plCamCalibrate		= nullptr;

	//////////////////////////////////////////////////
	//
	//		ALIGNMENT SETTINGS MENU
	//
	//////////////////////////////////////////////////
	UIPlate* inspAlignPlate		= nullptr;
	UIPlate* alignButtonGrid	= nullptr;
	UIPlate* alignSubOpacity	= nullptr;
	UIPlate* alignThreshold		= nullptr;

	UIVBulkSlider* sliderOpacity= nullptr;
	UIVBulkSlider* sliderThresh = nullptr;
	UIVBulkSlider* sliderDispup = nullptr;

	//////////////////////////////////////////////////
	//
	//		CAMERA SETTINGS MENU
	//
	//////////////////////////////////////////////////
	UIPlate* inspCamSetsPlate	= nullptr;

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
	void OnKeyup(wxKeyCode key) override;
	void OnMouseDown(int button, const wxPoint& pt) override;
	void OnMouseUp(int button, const wxPoint& pt) override;
	void OnMouseMove(const wxPoint& pt) override;
	//
	void Initialize() override;
	void ClosingApp() override;


	~StateHMDOp();

protected:
	void SetShownMenuBarUIPanel(int idx);

	void ApplyFormButtonStyle(UIGraphic* uib);
public:
	void OnUISink_Clicked(UIBase* uib, int mouseBtn, const UIVec2& mousePos) override;

};