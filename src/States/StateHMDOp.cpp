#include "StateHMDOp.h"
#include "StateIncludes.h"
#include "../CamVideo/CamStreamMgr.h"
#include "../Utils/cvgShapes.h"
#include "../Utils/cvgAssert.h"
#include "../UISys/UIPlate.h"
#include "../UISys/UIButton.h"
#include "../UISys/UIVBulkSlider.h"
#include <cmath>
#include <dcmtk/dcmdata/dcdeftag.h>

#include "HMDOpSubs/HMDOpSub_Carousel.h"
#include "HMDOpSubs/HMDOpSub_Default.h"
#include "HMDOpSubs/HMDOpSub_MainMenuNav.h"
#include "../Utils/cvgAssert.h"

// The standard colors for buttons
ColorSetInteractable colSetButton(
	UIColor4(1.0f, 1.0f, 1.0f),	// NORM:	White
	UIColor4(0.5f, 1.0f, 0.5f),	// HOVER:	Green
	UIColor4(0.7f, 1.0f, 0.7f),	// PRESSED:	Bright green
	UIColor4(0.7f, 1.0f, 0.5f));// SEL:		Orangeish

// The standard colors for buttons that are toggled on
ColorSetInteractable colSetButtonTog(
	UIColor4(1.0f, 0.2f, 0.2f),	// NORM:	Red
	UIColor4(1.0f, 0.5f, 0.5f),	// HOVER:	Pink
	UIColor4(1.0f, 0.7f, 0.7f), // PRESSED:	Bright pink
	UIColor4(1.0f, 0.8f, 0.5f));// SEL:		

void SetButtonStdCols(UIBase* uib, bool toggled = false)
{
	if(!toggled)
		uib->uiCols = colSetButton;
	else
		uib->uiCols = colSetButtonTog;
}


const float icoDim = 60.0f;					// (Square) Size of main menu icons/buttons in pixels
const float menuYPiv = 1.05f / 7.0f;		// Separation of main menu icons, in terms of percentage of the main menu bar.

const UIColor4 plateGray(0.5f, 0.5f, 0.5f, 1.0f);
const UIColor4 menuTitleCol(0.0f, 0.0f, 0.0f, 1.0f);
const float titleHeight = 40.0f;

// Level should go from 1 to 3
void SetLaserLevel(LaserSys& laser, int level)
{
	cvgAssert(level > 0, "Laser level too low, must be between [1,3]");
	cvgAssert(level <= 3, "Laser level high low, must be between [1,3]");

	float power = 0.0;
	switch(level)
	{
	case 1:
		power = 0.30f;
		break;

	case 2:
		power = 0.70f;
		break;

	case 3:
		power = 1.0f;
		break;
	}

	laser.SetDefault(
		LaserSys::Light::NIR,
		power,
		LaserSys::DefaultSetMode::OnlyIfOn);
}

StateHMDOp::StateHMDOp(HMDOpApp* app, GLWin* view, MainWin* core)
	:	BaseState(BaseState::AppState::MainOp, app, view, core),
		uiSys(-1, UIRect(0, 0, 1920, 1080), this),
		substateMachine(this)
{
	this->carouselStyle.SetOrientation(CarouselStyle::Orientation::Vertical);
	this->carouselStyle.center = true;

	CamStreamMgr& camMgr = CamStreamMgr::GetInstance();

	//////////////////////////////////////////////////
	//
	//		LOAD COMMON ASSETS
	//
	//////////////////////////////////////////////////
	this->texSliderSysPos = TexObj::MakeSharedLODE("Assets/Circle_Pos.png");
	this->texSliderSysNeg = TexObj::MakeSharedLODE("Assets/Circle_Neg.png");

	this->uiSys.SetSelectingButtons(true, true, true);

	this->patch_circle		= TexObj::MakeSharedLODE("Assets/UIPlates/8mmCircle.png"		);
	this->patch_roundLeft	= TexObj::MakeSharedLODE("Assets/UIPlates/8mmRoundedLeft.png"	);
	this->patch_roundRight	= TexObj::MakeSharedLODE("Assets/UIPlates/8mmRoundedRight.png"	);
	this->ninePatchCircle	= NinePatcher::MakeFromPixelsMiddle(this->patch_circle);

	this->patch_smallCircle	= TexObj::MakeSharedLODE("Assets/UIPlates/3mmCircle.png");
	this->ninePatchSmallCircle = NinePatcher::MakeFromPixelsMiddle(this->patch_smallCircle);

	
	//////////////////////////////////////////////////
	//
	//		CONSTRUCT RETAINED UI HEIRARCHY
	//
	//////////////////////////////////////////////////
	//
	// This large wall of code sets up the UI items
	// used in the application, as well as their starting
	// positions and drawing/behaviour states.
	// 
	const int btnTextSz = 14.0f;
	
	//		MAIN MENU BUTTONS
	//
	//////////////////////////////////////////////////

	// Build elements for the right menu bar
	//
	this->vertMenuPlate = new UIPlate( &this->uiSys, UIID::PullMenu, UIRect(960, 540, 50.0f, 100.0f), plateGray);
	this->vertMenuPlate->SetMode_Patch(this->patch_roundRight, this->ninePatchCircle);

	this->plateMainLaserBack = 
		new UIPlate(
			this->vertMenuPlate, -1, 
			UIRect(0, 0, 0, 0)); // Set in RefreshLaserBackPlate() call below
	//
	this->plateMainLaserBack->SetPivot(0.0f, 0.5f - menuYPiv * 2.0f);
	this->RefreshLaserBackPlate(false);
	this->SetLaserBackPlateColor(false);

	//
	this->btnLaser		= new UIButton(this->vertMenuPlate, UIID::MBtnLaserTog, UIRect(20.0f, -icoDim/2.0f, icoDim, icoDim),		"Assets/MenubarIcos/Menu_Icon_Laser.png"	);
	this->btnLaser->SetPivot(0.0f, 0.5f - menuYPiv * 2.0f);
	this->btnLaser->customFlags |= CustomUIFlag::IsMainOptWithNoContents;
	SetButtonStdCols(this->btnLaser);
	this->btnSettings	= new UIButton(this->vertMenuPlate, UIID::MBtnLaserSet, UIRect(20.0f, -icoDim/2.0f, icoDim, icoDim),		"Assets/MenubarIcos/Menu_Icon_Sliders.png");
	this->btnSettings->SetPivot(0.0f, 0.5f - menuYPiv * 0.5f);
	SetButtonStdCols(this->btnSettings);
	this->btnAlign		= new UIButton(this->vertMenuPlate, UIID::MBtnAlign,	UIRect(20.0f, -icoDim/2.0f, icoDim, icoDim),		"Assets/MenubarIcos/Menu_Icon_Align.png"	);
	this->btnAlign->SetPivot(0.0f, 0.5f + menuYPiv * 0.5f);
	SetButtonStdCols(this->btnAlign);
	this->btnCamSets	= new UIButton(this->vertMenuPlate, UIID::MBtnSource,	UIRect(20.0f, -icoDim/2.0f, icoDim, icoDim),		"Assets/MenubarIcos/Menu_Icon_Return.png"	);
	this->btnCamSets->SetPivot(0.0f, 0.5f + menuYPiv * 1.5f);
	SetButtonStdCols(this->btnCamSets);
	this->btnExit		= new UIButton(this->vertMenuPlate, UIID::MBtnExit,		UIRect(20.0f, -icoDim/2.0f, icoDim, icoDim * 0.5f),	"Assets/MenubarIcos/Menu_Icon_Exit.png"	)	;
	this->btnExit->SetPivot(0.0f, 0.5f + menuYPiv * 3.15f);
	SetButtonStdCols(this->btnExit);


	//		LASER SETTINGS INSPECTOR
	//
	//////////////////////////////////////////////////
	UIRect defInspPlateDim(800, 200, 400, 600);
	// Build elements for the inspector
	//
	this->inspSettingsPlate = new UIPlate( &this->uiSys, -1, defInspPlateDim, plateGray);
	inspSettingsPlate->SetMode_Patch(this->patch_roundLeft, this->ninePatchCircle);
	this->inspSettingsPlate->Show(false);
	//
	UIText* settingsTitle = new UIText(this->inspSettingsPlate, -1, "View Settings", 20, UIRect());
	settingsTitle->UseDyn()->AnchorsTop().SetOffsets(0.0f, 0.0f, 0.0f, titleHeight);
	settingsTitle->uiCols.SetAll(menuTitleCol);
	//
	this->inspSetFrame = new UIPlate(this->inspSettingsPlate, -1, UIRect());
	this->inspSetFrame->SetMode_Outline();
	this->inspSetFrame->UseDyn()->AnchorsTop().SetOffsets(10.0f, titleHeight, -10.0f, 280.0f);
	// TODO:
	// this->inspSetFrame->filled = false;
	this->inspSetFrame->SetAllColors(UIColor4(0.0f, 0.0f, 0.0f, 1.0f));
	UIText* textLasePwr = new UIText(this->inspSetFrame, -1, "LASE PWR", 20, UIRect());
	textLasePwr->UseDyn()->AnchorsTop().SetOffsets(0.0f, 0.0f, 0.0f, 40.0f);
	textLasePwr->uiCols.norm.SetColor_Black();
	this->btnLaseW_1 = new UIButton(this->inspSetFrame, UIID::LaseWat_1, UIRect(),	"30%", btnTextSz);
	this->btnLaseW_1->UseDyn()->SetAnchors(0.0f, 0.0f, 0.33f, 0.0f).SetOffsets(0.0f, 40.0f, 0.0f, 70.0f);
	this->ApplyFormButtonStyle(this->btnLaseW_1);
	SetButtonStdCols(this->btnLaseW_1, true);
	this->btnLaseW_1->customFlags |= CustomUIFlag::IsGroupStart;
	this->btnLaseW_2 = new UIButton(this->inspSetFrame, UIID::LaseWat_2, UIRect(),	"70%", btnTextSz);
	this->btnLaseW_2->UseDyn()->SetAnchors(0.33f, 0.0f, 0.66f, 0.0f).SetOffsets(0.0f, 40.0f, 0.0f, 70.0f);
	this->ApplyFormButtonStyle(this->btnLaseW_2);
	SetButtonStdCols(this->btnLaseW_2, false);
	this->btnLaseW_3 = new UIButton(this->inspSetFrame, UIID::LaseWat_3, UIRect(),	"100%", btnTextSz);
	this->btnLaseW_3->UseDyn()->SetAnchors(0.66f, 0.0f, 1.0f, 0.0f).SetOffsets(0.0f, 40.0f, 0.0f, 70.0f);
	this->ApplyFormButtonStyle(this->btnLaseW_3);
	SetButtonStdCols(this->btnLaseW_3, false);
	UIText* textLaseExp = new UIText(this->inspSetFrame, -1, "EXPOSURE", 20, UIRect());
	textLaseExp->UseDyn()->AnchorsTop().SetOffsets(0.0f, 100.0f, 0.0f, 140.0f);
	textLaseExp->uiCols.norm.SetColor_Black();
	this->btnExp_1	= new UIButton(this->inspSetFrame, UIID::Lase_Exposure_1, UIRect(20, 150, 50, 30), "E1", btnTextSz);
	this->btnExp_1->UseDyn()->SetAnchors(0.0f, 0.0f, 0.5f, 0.0f).SetOffsets(0.0f, 140.0f, 0.0f, 170.0f);
	this->ApplyFormButtonStyle(this->btnExp_1);
	SetButtonStdCols(this->btnExp_1, true);
	this->btnExp_1->customFlags |= CustomUIFlag::IsGroupStart;
	this->btnExp_2	= new UIButton(this->inspSetFrame, UIID::Lase_Exposure_2, UIRect(80, 150, 50, 30), "E2", btnTextSz);
	this->btnExp_2->UseDyn()->SetAnchors(0.5f, 0.0f, 1.0f, 0.0f).SetOffsets(0.0f, 140.0f, 0.0f, 170.0f);
	this->ApplyFormButtonStyle(this->btnExp_2);
	SetButtonStdCols(this->btnExp_2, false);
	this->btnExp_3	= new UIButton(this->inspSetFrame, UIID::Lase_Exposure_3, UIRect(20, 200, 50, 30), "E3", btnTextSz);
	this->btnExp_3->UseDyn()->SetAnchors(0.0f, 0.0f, 0.5f, 0.0f).SetOffsets(0.0f, 190.0f, 0.0f, 220.0f);
	this->ApplyFormButtonStyle(this->btnExp_3);
	SetButtonStdCols(this->btnExp_3, false);
	this->btnExp_4	= new UIButton(this->inspSetFrame, UIID::Lase_Exposure_4, UIRect(80, 200, 50, 30), "E4", btnTextSz);
	this->btnExp_4->UseDyn()->SetAnchors(0.5f, 0.0f, 1.0f, 0.0f).SetOffsets(0.0f, 190.0f, 0.0f, 220.0f);
	this->ApplyFormButtonStyle(this->btnExp_4);
	SetButtonStdCols(this->btnExp_4, false);
	
	UIText* threshTxt = new UIText(inspSettingsPlate, -1, "THRESHOLDING", 14, UIRect());
	threshTxt->SetAllColors(UIColor4(0.0f, 0.0f, 0.0f, 1.0f));
	threshTxt->UseDyn()->SetAnchors(0.0f, 0.0f, 1.0f, 0.0f).SetOffsets(0.0f, 300.0f, 0.0f, 330.0f);

	this->btnThreshSel_None = new UIButton(inspSettingsPlate, Lase_Threshold_None, UIRect(), "None", 14);
	this->btnThreshSel_None->UseDyn()->SetAnchors(0.0f, 0.0f, 1.0f, 0.0f).SetOffsets(0.0f, 330.0f, 0.0f, 360.0f);
	this->ApplyFormButtonStyle(this->btnThreshSel_None);
	SetButtonStdCols(this->btnThreshSel_None, false);
	this->btnThreshSel_None->customFlags |= CustomUIFlag::IsGroupStart;
	this->btnThreshTog_Simple = new UIButton(inspSettingsPlate, Lase_Threshold_Simple, UIRect(), "Simple", 14 );
	this->btnThreshTog_Simple->UseDyn()->SetAnchors(0.0f, 0.0f, 1.0f, 0.0f).SetOffsets(0.0f, 360.0f, 0.0f, 390.0f);
	this->ApplyFormButtonStyle(this->btnThreshTog_Simple);
	SetButtonStdCols(this->btnThreshTog_Simple, false);
	this->btnThreshTog_Mean2 = new UIButton(inspSettingsPlate, Lase_Threshold_Mean2, UIRect(), "Mean2", 14);
	this->btnThreshTog_Mean2->UseDyn()->SetAnchors(0.0f, 0.0f, 1.0f, 0.0f).SetOffsets(0.0f, 390.0f, 0.0f, 420.0f);
	this->ApplyFormButtonStyle(this->btnThreshTog_Mean2);
	SetButtonStdCols(this->btnThreshTog_Mean2, false);
	this->btnThreshTog_Yen = new UIButton(inspSettingsPlate, Lase_Threshold_Yen, UIRect(), "Yen", 14);
	this->btnThreshTog_Yen->UseDyn()->SetAnchors(0.0f, 0.0f, 1.0f, 0.0f).SetOffsets(0.0f, 420.0f, 0.0f, 450.0f);
	this->ApplyFormButtonStyle(this->btnThreshTog_Yen);
	SetButtonStdCols(this->btnThreshTog_Yen, false);
	this->btnThreshTog_YenSimple = new UIButton(inspSettingsPlate, Lase_Threshold_YenSimple, UIRect(), "Yen Simple", 14);
	this->btnThreshTog_YenSimple->UseDyn()->SetAnchors(0.0f, 0.0f, 1.0f, 0.0f).SetOffsets(0.0f, 450.0f, 0.0f, 480.0f);
	this->ApplyFormButtonStyle(this->btnThreshTog_YenSimple);
	SetButtonStdCols(this->btnThreshTog_YenSimple, false);
	
	//		ALIGNMENT SETTINGS INSPECTOR
	//
	//////////////////////////////////////////////////

	this->inspAlignPlate	= new UIPlate( &this->uiSys, -1, defInspPlateDim, plateGray);
	this->inspAlignPlate->SetMode_Patch(this->patch_roundLeft, this->ninePatchCircle);
	this->inspAlignPlate->Show(false);
	//
	UIText* alignTitle = new UIText(this->inspAlignPlate, -1, "Reregister", 20, UIRect());
	alignTitle->UseDyn()->AnchorsTop().SetOffsets(0.0f, 0.0f, 0.0f, titleHeight);
	alignTitle->uiCols.SetAll(menuTitleCol);

	//		CAMERA SETTINGS INSPECTOR
	//
	//////////////////////////////////////////////////
	const float BtnHPad = 10.0f;
	const float BtnVPad = 10.0f;
	const float BtnH = 40.0f;
	const float BtnStride = BtnH + BtnVPad;

	this->inspCamSetsPlate	= new UIPlate( &this->uiSys, -1, defInspPlateDim, plateGray);
	this->inspCamSetsPlate->SetMode_Patch(this->patch_roundLeft, this->ninePatchCircle);
	this->inspCamSetsPlate->Show(false);
	UIText* camSetsTitle = new UIText(this->inspCamSetsPlate, -1, "Cam. Settings", 20, UIRect());
	camSetsTitle->UseDyn()->AnchorsTop().SetOffsets(0.0f, 0.0f, 0.0f, titleHeight);
	camSetsTitle->uiCols.SetAll(menuTitleCol);
	{
		this->camButtonGrid = new UIPlate(this->inspCamSetsPlate, -1, UIRect(), plateGray);
		this->camButtonGrid->SetMode_Invisible();
		this->camButtonGrid->UseDyn()->AnchorsAll().SetOffsets(0.0f, titleHeight, 0.0f, 0.0f);
		this->camButtonGrid->Show(true);

		int btnIt = BtnVPad;
		this->camBtnExposure = new UIButton(this->camButtonGrid, UIID::CamSet_Exposure, UIRect(), "EXPOSURE", btnTextSz);
		this->ApplyFormButtonStyle(this->camBtnExposure);
		this->camBtnExposure->UseDyn()->AnchorsTop().SetOffsets(BtnHPad, btnIt, -BtnHPad, btnIt + BtnH);
		SetButtonStdCols(this->camBtnExposure);
		btnIt += BtnStride;
		this->camBtnDisparity = new UIButton(this->camButtonGrid, UIID::CamSet_Disparity, UIRect(), "DISPARITY", btnTextSz);
		this->ApplyFormButtonStyle(this->camBtnDisparity);
		this->camBtnDisparity->UseDyn()->AnchorsTop().SetOffsets(BtnHPad, btnIt, -BtnHPad, btnIt + BtnH);
		SetButtonStdCols(this->camBtnDisparity);
		btnIt += BtnStride;
		this->camBtnOpacity = new UIButton(this->camButtonGrid, UIID::CamSet_Opacity, UIRect(), "OPACITY", btnTextSz);
		this->ApplyFormButtonStyle(this->camBtnOpacity);
		this->camBtnOpacity->UseDyn()->AnchorsTop().SetOffsets(BtnHPad, btnIt, -BtnHPad, btnIt + BtnH);
		SetButtonStdCols(this->camBtnOpacity);
		{
			float curOpacity = 1.0f;
			// TODO: This part will need some"re-adjustment, at this point in the application,
			// the camera streams aren't available to query from.
			//int segCamIdx = this->GetView()->cachedOptions.FindMenuTargetIndex();
			//if(segCamIdx != -1)
			//	curOpacity = camMgr.GetParam(segCamIdx, StreamParams::Alpha);

			this->sliderSysOpacity = this->CreateSliderSystem(&this->uiSys, UIID::CamSet_Opacity_Meter, "Opacity", 0.0f, 1.0f, curOpacity, UIRect(), this->camBtnOpacity);
		}
		btnIt += BtnStride;
		this->camBtnRegisterXY = new UIButton(this->camButtonGrid, UIID::CamSet_Register, UIRect(), "REGISTER X/Y", btnTextSz);
		this->ApplyFormButtonStyle(this->camBtnRegisterXY);
		this->camBtnRegisterXY->UseDyn()->AnchorsTop().SetOffsets(BtnHPad, btnIt, -BtnHPad, btnIt + BtnH);
		SetButtonStdCols(this->camBtnRegisterXY);
		btnIt += BtnStride;
		this->camBtnCalibrate = new UIButton(this->camButtonGrid, UIID::CamSet_Calibrate, UIRect(), "Calibrate Focus", btnTextSz);
		this->ApplyFormButtonStyle(this->camBtnCalibrate);
		this->camBtnCalibrate->UseDyn()->AnchorsTop().SetOffsets(BtnHPad, btnIt, -BtnHPad, btnIt + BtnH);
		SetButtonStdCols(this->camBtnCalibrate);
		{
			this->sliderSysCalibrate = this->CreateSliderSystem(&this->uiSys, UIID::CamSet_Calibrate_Slider,	"Calibrate", 0.0f, 1.0f, 0.25f, UIRect(), this->camBtnCalibrate);
		}
		btnIt += BtnStride;
		this->camBtnThresh = new UIButton(this->camButtonGrid, UIID::CamSet_Threshold, UIRect(), "Threshold Settings", btnTextSz);
		this->ApplyFormButtonStyle(this->camBtnThresh);
		this->camBtnThresh->UseDyn()->AnchorsTop().SetOffsets(BtnHPad, btnIt, -BtnHPad, btnIt + BtnH);
		SetButtonStdCols(this->camBtnThresh);
		{
			this->sliderSysThresh = this->CreateSliderSystem(&this->uiSys, UIID::CamSet_Threshold_SlideThresh,	"Thresh", 0.0f, 255.0f, 127.0f, UIRect(), this->camBtnThresh);
			this->sliderSysDispUp = this->CreateSliderSystem(&this->uiSys, UIID::CamSet_Threshold_DispUp,		"DispUp", 0.0f, 1.0f, 0.25f, UIRect(), this->camBtnThresh);
		}
		//btnIt += BtnStride;
	}
	

	// ADD DEFAULT Substates
	this->substateMachine.CacheSubstate((int)CoreSubState::CarouselStage, StateHMDOp::SubPtr(new HMDOpSub_Carousel(this, &this->substateMachine)));
	this->substateMachine.CacheSubstate((int)CoreSubState::Default		, StateHMDOp::SubPtr(new HMDOpSub_Default(this, &this->substateMachine)));
	this->substateMachine.CacheSubstate((int)CoreSubState::MenuNav		, StateHMDOp::SubPtr(new HMDOpSub_MainMenuNav(this, &this->substateMachine)));

	//		EXIT INSPECTOR
	//
	//////////////////////////////////////////////////

	this->inspExitPlate	= new UIPlate( &this->uiSys, -1, defInspPlateDim, plateGray);
	this->inspExitPlate->SetMode_Patch(this->patch_roundLeft, this->ninePatchCircle);
	this->inspExitPlate->Show(false);
	//
	UIText* exitTitle = new UIText(this->inspExitPlate, -1, "Exit", 20, UIRect());
	exitTitle->UseDyn()->AnchorsTop().SetOffsets(0.0f, 0.0f, 0.0f, titleHeight);
	exitTitle->uiCols.SetAll(menuTitleCol);
	{
		int btnIt = BtnVPad + titleHeight;

		UIButton* exitBtnConfirm = new UIButton(this->inspExitPlate, UIID::Exit_Confirm, UIRect(), "CONFIRM", btnTextSz);
		this->ApplyFormButtonStyle(exitBtnConfirm);
		exitBtnConfirm->UseDyn()->AnchorsTop().SetOffsets(BtnHPad, btnIt, -BtnHPad, btnIt + BtnH);
		SetButtonStdCols(exitBtnConfirm);
	}

	//////////////////////////////////////////////////
	//
	//		FINALIZATION
	//
	//////////////////////////////////////////////////

	// Hide all the created stuff that shouldn't be visible yet.
	this->ManageCamButtonPressed(-1);

	// Set the laser level to what the UI was initialized to show is selected.
	SetLaserLevel(*this->GetCoreWindow()->hwLaser, 1);
	
}

void StateHMDOp::AddCamGroupingEntry(UIButton* button, UIPlate* system, UIHSlider* slider)
{
	this->AddCamGroupingEntry(button, {system, slider});
}

void StateHMDOp::AddCamGroupingEntry(UIButton* button, PlateSliderPair pair)
{
	this->camButtonGrouping[button].push_back(pair);
}

void StateHMDOp::DrawRecordingDot(float x, float y, float rad)
{
	double strobe = (double)clock() / CLOCKS_PER_SEC;
	strobe = (sin(strobe) + 1.0f)/2.0f * 0.5f + 0.5f; // [0.5, 1.0]
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glColor3f(strobe * 1.0f, strobe * 0.25f, strobe * 0.25f);
	const int RecCircPts = 32;
	
	// This would probably be cheaper if we switch this to a triangle fan
	glBegin(GL_TRIANGLES);
	for(int i = 0; i < RecCircPts; ++i)
	{
		const float PI = 3.14159f;
		float th0 = (float)(i + 0) / RecCircPts;
		float th1 = (float)(i + 1) / RecCircPts;
		th0 *= 2.0f * PI;
		th1 *= 2.0f * PI;
		float x0 = cos(th0);
		float y0 = sin(th0);
		float x1 = cos(th1);
		float y1 = sin(th1);
		glVertex2f(x, y);
		glVertex2f( x + x1 * rad, y + y1 * rad);
		glVertex2f( x + x0 * rad, y + y0 * rad);

	}
	glEnd();

	// NOTE: Should we draw a "REC" inside of it?
}

void StateHMDOp::Draw(const wxSize& sz)
{	
	CamStreamMgr& camMgr = CamStreamMgr::GetInstance();
	float cx = sz.x / 2;
	float cy = sz.y / 2;

	float vrWidth = (float)this->GetView()->viewportX;
	float vrHeight = (float)this->GetView()->viewportY;
	float vrOffsX = (float)this->GetView()->viewportOfsX;
	float vrOffsY = (float)this->GetView()->viewportOfsY;

	cvgRect cameraWindowRgn = 
		cvgRect(
			vrOffsX + cx - vrWidth * 0.5f, 
			vrOffsY + cy - vrHeight * 0.5f, 
			vrWidth, 
			vrHeight);

	//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive blending

	// DRAW THE CAMERAS ADDITIVELY COMPOSITED ON TOP OF EACH OTHER
	for(int camIt = 0; camIt < 2; ++camIt)
	{
		cv::Ptr<cv::Mat> curImg = camMgr.GetCurrentFrame(camIt);
		if(curImg == nullptr)
			continue;
	
		long long feedCtr = camMgr.GetCameraFeedChanges(camIt);
		this->camTextureRegistry.LoadTexture(camIt, curImg, feedCtr);
	
		cvgCamTextureRegistry::Entry texInfo = 
			this->camTextureRegistry.GetInfoCopy(camIt);
	
		if(texInfo.IsEmpty())
			continue;

		// The image can keep its aspect ratio, but it should respect the
		// region dedicated for rendering camera feeds (cameraWindowRgn).
		float vaspect = 0.0f;
		if(texInfo.cachedWidth != 0.0f)
			vaspect = (float)texInfo.cachedHeight / (float)texInfo.cachedWidth;

		cvgRect viewRegion = cvgRect::MakeWidthAspect(cameraWindowRgn.w, vaspect);
		viewRegion.x = vrOffsX + cx - viewRegion.w * 0.5f;
		viewRegion.y = vrOffsY + cy - viewRegion.h * 0.5f;

		glBindTexture(GL_TEXTURE_2D, texInfo.glTexId);

		float alpha = camMgr.GetParam(camIt, StreamParams::Alpha);
		//if(camMgr.IsThresholded(camIt))
		//	glColor4f(1.0f, 0.0f, 0.0f, alpha);
		//else
			glColor4f(1.0f, 1.0f, 1.0f, alpha);

		glBegin(GL_QUADS);
			viewRegion.GLVerts_Textured();
		glEnd();
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
	
	this->DrawMenuSystemAroundRect(cameraWindowRgn);

	// Draw debug timings
	if(UISys::IsDebugView())
	{ 
		const int camCt = 2;
		for(int i = 0; i < camCt; ++i)
		{
			std::stringstream sstrm;
			sstrm << "Cam: " << i << " - MS: " << camMgr.GetMSFrameTime(i);
			this->fontInsTitle.RenderFont(sstrm.str().c_str(), 0, sz.y - (20 * camCt) + (20 * i));
		}
	}

	this->vertMenuPlate->SetLocPos(cameraWindowRgn.EndX() + 10.0f, cameraWindowRgn.y + 25.0f);
	this->vertMenuPlate->SetDim(this->curVertWidth, cameraWindowRgn.h - 50.0f);

	glEnable(GL_BLEND);
	this->uiSys.AlignSystem();
	this->uiSys.Render();

	if(
		this->curVertWidth == this->minVertWidth && 
		this->GetCoreWindow()->hwLaser->intensityNIR != 0)
	{
		// If the main menu is compressed and the laser is on, add a little more
		// emphasis on the laser backplate. 
		//
		// Specifically, strobing letters that say LASER ON in the vertical space.
		UIRect rectLaserBackplate = this->plateMainLaserBack->GetGlobalRect();
		glPushMatrix();
			const float textMargin = 7.5f;
			glTranslatef(rectLaserBackplate.pos.x + textMargin, rectLaserBackplate.pos.y + textMargin, 0.0f);
			glRotatef(90.0f, 0.0f, 0.0f, 1.0f);

			// Create a smooth oscillating value between [0.0, 1.0]
			const double strobeFreqMod = 1.0;
			float strobePhase = (sin((double)clock() / (double)CLOCKS_PER_SEC * strobeFreqMod) + 1.0f) * 0.5f;
			static const UIColor4 strobeLasOnTxtA(1.0f, 0.2f, 0.2f);
			static const UIColor4 strobeLasOnTxtB(1.0f, 0.5f, 0.5f);
			//
			UIColor4 strobeUse = UIColor4::Lerp(strobeLasOnTxtA, strobeLasOnTxtB, strobePhase);
			strobeUse.GLColor3();
			this->fontLaserOn.RenderFont("LASER ON", 0.0f, 0.0f);
		glPopMatrix();
		
	}

	if(this->showCarousel)
	{ 
		Carousel* sel = this->GetSelectedCarousel();
		const static UIColor4 selCaroColor(1.0f, 1.0f, 1.0f);
		const static UIColor4 unselCaroColor(0.5f, 0.5f, 0.5f);

		const float CarouselY = 850.0f;

		this->caroOrient.Render(	
			sz.x * 0.5f,		
			CarouselY - this->carouselLiftAmts[(int)CarouselType::Orient], 
			this->carouselStyle, 
			1.0f,
			(&this->caroOrient == sel) ? selCaroColor : unselCaroColor);

		this->caroBody.Render(
			sz.x * 0.5f + 200,	
			CarouselY - this->carouselLiftAmts[(int)CarouselType::Study],
			this->carouselStyle, 
			1.0f,
			(&this->caroBody == sel) ? selCaroColor : unselCaroColor);

		this->caroSeries.Render(
			sz.x * 0.5f - 200,	
			CarouselY - this->carouselLiftAmts[(int)CarouselType::Series],
			this->carouselStyle, 
			1.0f,
			(&this->caroSeries == sel) ? selCaroColor : unselCaroColor);

		
	}

	float mousepadX = sz.x /2 + (float)this->GetView()->mousepadOffsX;		// Horizontally at the center
	float mousepadY = sz.y / 2 + (float)this->GetView()->mousepadOffsY;		// Near the bottom
	//
	this->mousepadUI.Render(
		(HMDOpSub_Base*)this->substateMachine.GetCurSubtate().get(),
		mousepadX,
		mousepadY,
		(float)this->GetView()->mousepadScale);

	if(camMgr.IsRecording(SpecialCams::Composite))
	{
		const float RecOffX = 270;
		const float RecOffY = -65;
		const float RecCircRad = 16.0f;
		this->DrawRecordingDot(mousepadX + RecOffX, mousepadY + RecOffY, RecCircRad);
	}

	if(UISys::IsDebugView())
	{ 
		glColor3f(1.0f, 0.0f, 1.0f);

		// Debug of the states in the substateMachine stack.
		// We render with the current substate at the top.
		int yDbgRender = 50;
		int subDepth = this->substateMachine.GetStackDepth();
		for(int i = subDepth - 1; i >= 0; --i )
		{
			std::string subName = this->substateMachine.PeekNameOfDepth(i);
			if(i == subDepth - 1)
				this->fontInsTitle.RenderFont(subName.c_str(), 50, yDbgRender);
			else
			{
				// Indent the non-topmost to visually supress them and 
				// convey how they're not the current state.
				this->fontInsTitle.RenderFont(subName.c_str(), 75, yDbgRender);
			}

			yDbgRender += 20;
		}
	}
	
}

// NOTE: This function has been greatly reduce from its original purpose.
// It may be the case we remove this function, or change its name.
void StateHMDOp::DrawMenuSystemAroundRect(const cvgRect& rectDrawAround)
{
	cvgRect cvgrOutl = rectDrawAround;
	cvgrOutl.Inflate(10.0f);
	glDisable(GL_TEXTURE_2D);
	glColor3f(0.5f, 0.5f, 0.5f);
	glBegin(GL_LINE_LOOP);
		cvgrOutl.GLVerts();
	glEnd();

	// Because of some weird history involving the code, we have two
	// different representation of rectangle bounds...
	UIRect r(
		cvgrOutl.x, 
		cvgrOutl.y, 
		cvgrOutl.w, 
		cvgrOutl.h);

	if(this->lastViewportRect != r)
	{
		this->lastViewportRect = r;

		// There's some +- 1.0s and 2.0s to make sure we don't
		// draw over lines
		UIRect inspLoc(
			r.Right() - r.dim.x * 0.2f - 2.0f,
			r.pos.y + 1.0f,
			r.dim.x * 0.2f,
			r.dim.y - 2.0f);

		this->inspSettingsPlate->SetRect(inspLoc);
		this->inspAlignPlate->SetRect(inspLoc);
		this->inspCamSetsPlate->SetRect(inspLoc);
		this->inspExitPlate->SetRect(inspLoc);

		const float lerpLeft = 0.1f;
		const float lerpWidth = 0.6f;
		const float rowHeight = 40.0f;
		const float rowOffs = 50.0f;

		for(auto itCBG : this->camButtonGrouping)
		{
			const std::vector<PlateSliderPair>& vecGroup = itCBG.second;

			// For each group, recalculate the layout of their children sliders
			// with respect to the new viewport rect properties.
			for(int i = 0; i < vecGroup.size(); ++i)
			{
				vecGroup[i].plate->SetRect(
					r.LerpHoriz(lerpLeft),  
					r.Bottom() - 20.0f + i * rowOffs, 
					r.LerpWidth(lerpWidth), 
					rowHeight);
			}
		}
	}
}


void StateHMDOp::Update(double dt)
{
	//if(this->inspectorShow == true)
	//{
	//	// TODO: Slide in/out inspector
	//}

	bool showRButtons = false;
	if(this->showMainMenu)
	{ 
		curVertWidth = (float)std::min<float>(maxVertWidth, curVertWidth + vertTransSpeed * dt);
		showRButtons = (curVertWidth == maxVertWidth);

		this->RefreshLaserBackPlate(true);
		this->HideCarousels();
	}
	else
	{
		this->RefreshLaserBackPlate(false);
		this->curVertWidth = (float)std::max<float>(minVertWidth, curVertWidth - vertTransSpeed * dt);
	}

	this->btnLaser->Show(showRButtons);
	this->btnSettings->Show(showRButtons);
	this->btnAlign->Show(showRButtons);
	this->btnCamSets->Show(showRButtons);
	this->btnExit->Show(showRButtons);

	this->mousepadUI.Update(dt);

	if(this->showCarousel)
	{ 
		// Update individual animations
		this->caroBody.Update(this->carouselStyle, dt);
		this->caroSeries.Update(this->carouselStyle, dt);
		this->caroOrient.Update(this->carouselStyle, dt);

		// Update lifting and lowering for seleted carousel
		for(int i = 0; i < (int)CarouselType::Totalnum; ++i)
		{
			if( i == (int)this->selectedCarousel)
				this->carouselLiftAmts[i] = std::min((float)(this->carouselLiftAmts[i] + dt * CarouselChangeRate), MaxCarouselLift);
			else
				this->carouselLiftAmts[i] = std::max((float)(this->carouselLiftAmts[i] - dt * CarouselChangeRate), 0.0f);
		}
	}
}

void StateHMDOp::EnteredActive()
{
	
	// While these texture loads were initially in the App's Initialize(),
	// they've been moved to EnteredActive to deffer them, because
	// there was some texture loading issues on the Linux side -
	//
	// most likely because the OpenGL context doesn't get initialized
	// as fast as on Windows.
	this->mousepadUI.Initialize();
	this->mousepadUI.Reset();

	this->_SyncImageProcessingSetUI();
	this->_SyncThresholdSlider();

	CamStreamMgr& cmgr = CamStreamMgr::GetInstance();

	// Sync the composite saving resolution
	cmgr.SetParam(
		SpecialCams::Composite, 
		StreamParams::CompositeVideoWidth,
		this->GetView()->cachedOptions.compositeWidth);

	cmgr.SetParam(
		SpecialCams::Composite,
		StreamParams::CompositeVideoHeight,
		this->GetView()->cachedOptions.compositeHeight);

	this->substateMachine.ChangeCachedSubstate((int)CoreSubState::Default);


	this->_LoadExposureButtonLabelsFromOptions();	// Setup labels of exposure buttons
	this->GetView()->RefreshExposureSetting();		//		Re-enforce the value.
	this->_HighlighExposureButton();				//		Properly visually toggle the correct button.
} 

UIButton* StateHMDOp::GetExposureButton(int idx, bool mustBeValidIdx )
{
	switch(idx)
	{
	case 0:
		return this->btnExp_1;
	case 1:
		return this->btnExp_2;
	case 2:
		return this->btnExp_3;
	case 3:
		return this->btnExp_4;
	default:
		if(mustBeValidIdx)
			cvgAssert(false, "Unhandled Exposure button index for assigning a label.");
	}
	return nullptr;
}

void StateHMDOp::_LoadExposureButtonLabelsFromOptions()
{
	cvgAssert(
		this->btnExp_1 && this->btnExp_2 && this->btnExp_3 && this->btnExp_4, 
		"Expected Exposure buttons not ready.");

	for(int i = 0; i < SUPPORTED_EXPOSURE_ENTRIES; ++i)
	{
		cvgOptions::ExposureSetting eset = this->GetView()->cachedOptions.GetExposureEntry(i);
		UIButton* uib = this->GetExposureButton(i, true);
		
		if(uib == nullptr)
			continue;

		if(eset.label.empty())
			uib->text = "--";
		else
			uib->text = eset.label;
	}
}

void StateHMDOp::_HighlighExposureButton()
{
	cvgAssert(
		this->btnExp_1 && this->btnExp_2 && this->btnExp_3 && this->btnExp_4, 
		"Expected Exposure buttons not ready.");

	int activeExp = this->GetView()->GetUsedExposureSettingIdx();

	UIButton* uib = this->GetExposureButton(activeExp, false);
	int uiIdx = -1;
	if(uib != nullptr)
		uiIdx = uib->Idx();

	UpdateGroupColorSet(
		uiIdx, 
		{this->btnExp_1, this->btnExp_2, this->btnExp_3, this->btnExp_4}, 
		colSetButtonTog,
		colSetButton);
}

void StateHMDOp::_SyncImageProcessingSetUI()
{
	// Initialize the buttons states for the thresholding options
	// to match the app's current state.
	int targIdx = this->GetView()->cachedOptions.FindMenuTargetIndex();
	if(targIdx == -1)
		return;

	CamStreamMgr& cmgr = CamStreamMgr::GetInstance();
	ProcessingType pt = cmgr.GetProcessingType(targIdx);
	switch(pt)
	{
	case ProcessingType::None:
		this->DoThresholdButton(UIID::Lase_Threshold_None, ProcessingType::None, true);
		break;

	case ProcessingType::static_threshold:
		this->DoThresholdButton(UIID::Lase_Threshold_Simple, ProcessingType::static_threshold, true);
		break;

	case ProcessingType::two_stdev_from_mean:
		this->DoThresholdButton(UIID::Lase_Threshold_Mean2, ProcessingType::two_stdev_from_mean, true);
		break;

	case ProcessingType::yen_threshold:
		this->DoThresholdButton(UIID::Lase_Threshold_Yen, ProcessingType::yen_threshold, true);
		break;

	case ProcessingType::yen_threshold_compressed:
		this->DoThresholdButton(UIID::Lase_Threshold_YenSimple, ProcessingType::yen_threshold_compressed, true);
		break;
	}
}

void StateHMDOp::_SyncThresholdSlider()
{
	// NOTE: We may change this to a function that syncronizes everything
	int targIdx = this->GetView()->cachedOptions.FindMenuTargetIndex();
	if(targIdx == -1)
		return;

	CamStreamMgr& cmgr = CamStreamMgr::GetInstance();
	this->sliderSysThresh.slider->SetCurValue(cmgr.GetParam(targIdx, StreamParams::StaticThreshold));
}

void StateHMDOp::ExitedActive() 
{
	this->camTextureRegistry.ClearTextures();
	this->uiSys.DelegateReset();
	this->substateMachine.ForceExitSubstate();
}

void StateHMDOp::Initialize() 
{
	this->fontInsTitle = FontMgr::GetInstance().GetFont(24);
	this->fontLaserOn = FontMgr::GetInstance().GetFont(12);

	this->caroBody.Append(this->GetView()->cachedOptions.caroBody);
	this->caroBody.LoadAssets();
	this->caroSeries.Append(this->GetView()->cachedOptions.caroSysSeries);
	this->caroSeries.LoadAssets();
	this->caroOrient.Append(this->GetView()->cachedOptions.caroSysOrient);
	this->caroOrient.LoadAssets();
}

void StateHMDOp::OnKeydown(wxKeyCode key)
{
	//this->uiSys.DelegateKeydown(key);
}

void StateHMDOp::OnKeyup(wxKeyCode key)
{
	//this->uiSys.DelegateKeyup(key);
}

void StateHMDOp::OnMouseDown(int button, const wxPoint& pt)
{
	// The delegation to the UI system to handle mouse interaction
	// has been removed. Now it's exclusively the substates' job to
	// manage this interaction.
	StateHMDOp::SubPtr curSub = this->substateMachine.GetCurSubtate();
	this->mousepadUI.OnButtonDown((HMDOpSub_Base*)curSub.get(), button);
}

void StateHMDOp::OnMouseUp(int button, const wxPoint& pt)
{
	// The delegation to the UI system to handle mouse interaction
	// has been removed. Now it's exclusively the substates' job to
	// manage this interaction.
	StateHMDOp::SubPtr curSub = this->substateMachine.GetCurSubtate();
	this->mousepadUI.OnButtonUp((HMDOpSub_Base*)curSub.get(), button);
}

void StateHMDOp::OnMouseMove(const wxPoint& pt)
{
	// The delegation to the UI system to handle mouse interaction
	// has been removed. Now it's exclusively the substates' job to
	// manage this interaction.
	//
	// No mousemove logic ATM.
}

void StateHMDOp::ClosingApp() 
{
	// Get rid of icons while we know the OpenGL context is still
	// alive.
	this->mousepadUI.Shutdown();
}

StateHMDOp::~StateHMDOp()
{
}

bool StateHMDOp::ShowCarousels( bool show)
{
	if(this->showCarousel == show)
		return false;

	// Reset if shown, that way they get an extra animation to emphasize the
	// current selected item. Plus we don't have to worry about a paused
	// animation spurriously resuming itself.
	for(int i = 0; i < (int)CarouselType::Totalnum; ++i)
		this->carouselLiftAmts[i] = 0.0f;

	this->showCarousel = show;
	this->caroBody.EndAnimation(this->carouselStyle, true);
	this->caroSeries.EndAnimation(this->carouselStyle, true);
	this->caroOrient.EndAnimation(this->carouselStyle, true);

	return true;
}

bool StateHMDOp::HideCarousels()
{
	return this->ShowCarousels(false);
}

bool StateHMDOp::ToggleCarousels()
{
	if(this->showCarousel)
		return this->HideCarousels();
	else
	{ 
		this->uiSys.ClearCustomTabOrder();
		return this->ShowCarousels();
	}
}

Carousel* StateHMDOp::GetSelectedCarousel()
{
	switch(this->selectedCarousel)
	{
	case CarouselType::Study:
		return &this->caroBody;
	case CarouselType::Series:
		return &this->caroSeries;
	case CarouselType::Orient:
		return &this->caroOrient;
	}

	return nullptr;
}

const Carousel* StateHMDOp::GetSelectedCarousel() const
{
	switch(this->selectedCarousel)
	{
	case CarouselType::Study:
		return &this->caroBody;
	case CarouselType::Series:
		return &this->caroSeries;
	case CarouselType::Orient:
		return &this->caroOrient;
	}

	return nullptr;
}

bool StateHMDOp::DoesCarouselHaveMoreOnLeft() const
{ 
	const Carousel* selectedCarousel = this->GetSelectedCarousel();
	if(!selectedCarousel)
		return false;

	return selectedCarousel->AnyMoreOnLeft(); 
}

bool StateHMDOp::DoesCarouselHaveMoreOnRight() const
{ 
	const Carousel* selectedCarousel = this->GetSelectedCarousel();
	if(!selectedCarousel)
		return true;

	return selectedCarousel->AnyMoreOnRight(); 
}

bool StateHMDOp::IsSelectedCarouselAtStart() const
{ 
	const Carousel* selectedCarousel = this->GetSelectedCarousel();
	if(!selectedCarousel)
		return true;

	return selectedCarousel->AtStart();
}

bool StateHMDOp::IsSelectedCarouselAtEnd() const
{ 
	const Carousel* selectedCarousel = this->GetSelectedCarousel();
	if(!selectedCarousel)
		return true;

	return selectedCarousel->AtEnd();
}

bool StateHMDOp::MoveSelectedCarouselLeft()
{
	// This will change carousels, which is how we get the study/series/orientation
	// info the dicom - which needs thread saftey.
	std::lock_guard<std::mutex> guard(this->dicomMutex);

	Carousel* selectedCarousel = this->GetSelectedCarousel();
	if(!selectedCarousel)
		return false;

	bool ret = selectedCarousel->GotoPrev();
	if(ret)
		this->OnCarouselChanged(this->selectedCarousel);

	return ret;
}

bool StateHMDOp::MoveSelectedCarouselRight(bool wrap)
{
	// This will change carousels, which is how we get the study/series/orientation
	// info the dicom - which needs thread saftey.
	std::lock_guard<std::mutex> guard(this->dicomMutex);

	Carousel* selectedCarousel = this->GetSelectedCarousel();
	if(!selectedCarousel)
		return false;

	bool ret = selectedCarousel->GotoNext();
	if(!ret)
		ret = selectedCarousel->Goto(0, false);

	if(ret)
		this->OnCarouselChanged(this->selectedCarousel);

	return ret;
}

void StateHMDOp::OnCarouselChanged(CarouselType caroTy)
{
	// Currently nothing needs to be done. Everythign is handled on image
	// save with the dicom injector.
	switch(caroTy)
	{
	case CarouselType::Study:
		break;

	case CarouselType::Series:
		break;

	case CarouselType::Orient:
		break;
	}
}

void StateHMDOp::SelectNextCarousel(bool next)
{
	if(!next)
	{
		// Move to the previous instead of next
		// The extra addition is to handle if it goes to -1, to let it wrap
		// to the end.
		this->selectedCarousel = 
			(CarouselType)(((int)this->selectedCarousel - 1 + (int)CarouselType::Totalnum) % (int)CarouselType::Totalnum);
	}
	else
	{ 
		this->selectedCarousel = 
			(CarouselType)(((int)this->selectedCarousel + 1) % (int)CarouselType::Totalnum);
	}
}

void StateHMDOp::SetShownMenuBarUIPanel(int idx)
{
	bool toggleOff = false;

	// TODO: when turning off, we eventually need to handle a slide-in 
	// animation for the current active plate being disabled.

	// Regardless of toggling everything off, or toggling everything off
	// except the new context, we're going to turn it all off.
	this->inspSettingsPlate->Hide();
	this->inspCamSetsPlate->Hide();
	this->inspAlignPlate->Hide();
	this->inspExitPlate->Hide();

	// ... Else, it's a toggle on, so we turn off everything except for the one
	// thing we've switched our UI context to.

	UpdateGroupColorSet( 
		idx,
		{this->btnSettings, this->btnAlign, this->btnCamSets, this->btnExit},
		colSetButtonTog,
		colSetButton);

	switch(idx)
	{
	case UIID::MBtnLaserSet:
		this->inspSettingsPlate->Show();
		this->ManageCamButtonPressed(-1); // Turn all cam stuff off
		break;

	case UIID::MBtnAlign:
		this->inspAlignPlate->Show();
		this->ManageCamButtonPressed(-1); // Turn all cam stuff off
		break;

	case UIID::MBtnSource:
		this->inspCamSetsPlate->Show();
		this->ManageCamButtonPressed(-1);
		break;

	case UIID::MBtnExit:
		this->inspExitPlate->Show();
		this->ManageCamButtonPressed(-1);
	}
}

void StateHMDOp::ApplyFormButtonStyle(UIGraphic* uib)
{
	uib->SetMode_Patch(
		this->patch_smallCircle, 
		this->ninePatchSmallCircle);
}

void StateHMDOp::ManageCamButtonPressed(int buttonID)
{
	// Hide everything that could be shown as a horizontal bar.
	// If we need to see it again, it will be turned on again before
	// the function exits...
	for(auto it : this->camButtonGrouping)
	{
		for(PlateSliderPair& psp : it.second)
			psp.plate->Hide();
	}


	// The same goes for these toggle buttons
	std::vector<UIButton*> camSettingsCtxSet = 
	{
		this->camBtnExposure,
		this->camBtnDisparity,
		this->camBtnOpacity,
		this->camBtnCalibrate,
		this->camBtnRegisterXY,
		this->camBtnThresh
	};
	//
	for(UIButton* btn : camSettingsCtxSet)
	{
		SetButtonStdCols(btn, false);
	}

	this->uiSys.ClearCustomTabOrder();

	// Note we manually select the relevant button. Normally this isn't
	// needed for actual UI handling (i.e., if the user actually clicks
	// on the button) but becomes useful if we simulate clicking.
	UIButton* btnToShow = nullptr;
	switch(buttonID)
	{
		case CamSet_Exposure:
			btnToShow = this->camBtnExposure;
			break;
		case CamSet_Disparity:
			btnToShow = this->camBtnDisparity;
			break;
		case CamSet_Opacity:
			btnToShow = this->camBtnOpacity;
			break;
		case CamSet_Register:
			btnToShow = this->camBtnRegisterXY;
			break;
		case CamSet_Calibrate:
			btnToShow = this->camBtnCalibrate;
			break;
		case CamSet_Threshold:
			btnToShow = this->camBtnThresh;
			break;
	}

	if(btnToShow)
	{
		// TODO: Show context-specific children content when it exists.
		auto itFind = this->camButtonGrouping.find(btnToShow);
		std::vector<UIBase*> customTab = {btnToShow};
		if(itFind != this->camButtonGrouping.end())
		{
			std::vector<PlateSliderPair>& childContextSliders = itFind->second;
			for(PlateSliderPair& psp : childContextSliders)
			{ 
				psp.plate->Show();
				customTab.push_back(psp.slider);
			}
		}
		SetButtonStdCols(btnToShow, true);
		this->uiSys.Select(btnToShow);
		this->uiSys.SetCustomTabOrder(customTab);
	}
}

UIPlate* StateHMDOp::CreateSliderSystem(
	UIBase* parent, 
	int id,
	const std::string& labelText,
	float minVal,
	float maxVal,
	float startingVal,
	const UIRect& r,
	UIHSlider** outSlider)
{
	struct $_
	{
		static void UpdateText(UIHSlider * slider, UIText* text)
		{
			std::stringstream sstrm;
			sstrm << std::fixed << std::setprecision(3) << slider->GetValue(UIHSlider::VID::Value);
			text->SetText(sstrm.str());
		}
	};

	UIPlate* retPlate = new UIPlate(parent, -1, r);
	retPlate->SetAllColors(plateGray);

	UIText* label = new UIText(retPlate, -1, labelText, 14, UIRect());
	label->UseDyn()->SetAnchors(0.0f, 0.0f, 0.25f, 1.0f).ZeroOffsets();
	label->SetAllColors(UIColor4(1.0f, 1.0f, 1.0f, 1.0f));
	*label << HTextAlign::Right << VTextAlign::Center;

	UIText* valText = new UIText(retPlate, -1, "---", 14, UIRect());
	valText->UseDyn()->SetAnchors(0.75f, 0.0f, 1.0f, 1.0f).ZeroOffsets();
	*valText << HTextAlign::Left << VTextAlign::Center;
	valText->SetAllColors(UIColor4(1.0f, 1.0f, 1.0f, 1.0f));

	UIHSlider* slider = new UIHSlider(retPlate, id, minVal, maxVal, startingVal, UIRect(), this->patch_circle);
	slider->UseDyn()->SetAnchors(0.25f, 0.0f, 0.75f, 1.0f).SetOffsets(5.0f, 5.0f, -5.0f, -5.0f);
	slider->onSlide = [slider, valText](float){$_::UpdateText(slider, valText);};
	SetButtonStdCols(slider, false);

	const float decorDim = 20.0f;

	UIPlate* decorNeg = new UIPlate(retPlate, -1, UIRect());
	decorNeg->UseDyn()->SetAnchors(0.25f, 0.0f, 0.25f, 0.0f).SetOffsets(0.0f, 0.0f, decorDim, decorDim);
	decorNeg->SetAllColors(UIColor4(1.0f, 1.0f, 1.0f, 1.0f));
	decorNeg->SetMode_TexRect(this->texSliderSysNeg);

	UIPlate* decorPos = new UIPlate(retPlate, -1, UIRect());
	decorPos->UseDyn()->SetAnchors(0.75f, 0.0f, 0.75f, 0.0f).SetOffsets(-decorDim, 0.0f, 0.0f, decorDim);
	decorPos->SetAllColors(UIColor4(1.0f, 1.0f, 1.0f, 1.0f));
	decorPos->SetMode_TexRect(this->texSliderSysPos);


	$_::UpdateText(slider, valText);

	if(outSlider != nullptr)
		*outSlider = slider;

	return retPlate;
}

StateHMDOp::PlateSliderPair StateHMDOp::CreateSliderSystem(
	UIBase* parent, 
	int id,
	const std::string& labelText,
	float minVal,
	float maxVal,
	float startingVal,
	const UIRect& r,
	UIButton* btnCategory)
{
	PlateSliderPair ret;
	ret.plate = 
		this->CreateSliderSystem(
			parent,
			id,
			labelText,
			minVal,
			maxVal,
			startingVal,
			r,
			&ret.slider);

	if(btnCategory != nullptr)
		this->AddCamGroupingEntry(btnCategory, ret);

	return ret;
}

void StateHMDOp::DoThresholdButton(int idxButton, ProcessingType type, bool skipSet)
{
	int targIdx = this->GetView()->cachedOptions.FindMenuTargetIndex();
	if(skipSet || targIdx != -1)
	{
		CamStreamMgr& cmgr = CamStreamMgr::GetInstance();
		if(skipSet || cmgr.SetProcessingType(targIdx, type))
		{
			UpdateGroupColorSet(
				idxButton, 
				{
					this->btnThreshSel_None, 
					this->btnThreshTog_Simple, 
					this->btnThreshTog_Mean2, 
					this->btnThreshTog_Yen,
					this->btnThreshTog_YenSimple
				}, 
				colSetButtonTog,
				colSetButton);
		}
	}
}

void StateHMDOp::OnUISink_Clicked(UIBase* uib, int mouseBtn, const UIVec2& mousePos)
{
	
	// Only right click is supported
	if(mouseBtn != (int)MouseBtn::Right)
		return;

	int uiId = uib->Idx();
	switch(uiId)
	{
	case UIID::MBtnLaserTog:
		{
			if(this->GetCoreWindow()->hwLaser->intensityNIR == 0.0f)
			{ 
				this->GetCoreWindow()->hwLaser->ShowNIR();
				SetButtonStdCols(this->btnLaser, true);
				this->SetLaserBackPlateColor(true);
			}
			else
			{ 
				this->GetCoreWindow()->hwLaser->HideNIR();
				SetButtonStdCols(this->btnLaser, false);
				this->SetLaserBackPlateColor(false);
			}
		}
		break;

	case UIID::MBtnLaserSet:
		//this->SetShownMenuBarUIPanel(uib->Idx());
		break;

	case UIID::MBtnAlign:
		this->SetShownMenuBarUIPanel(uib->Idx());
		break;

	case UIID::MBtnSource:
		this->SetShownMenuBarUIPanel(uib->Idx());
		break;

	case UIID::MBtnExit:
		this->SetShownMenuBarUIPanel(uib->Idx());
		break;

	case UIID::LaseWat_1:
		UpdateGroupColorSet(
			uib->Idx(), 
			{this->btnLaseW_1, this->btnLaseW_2, this->btnLaseW_3}, 
			colSetButtonTog,
			colSetButton);

		SetLaserLevel(*this->GetCoreWindow()->hwLaser, 1);
		break;

	case UIID::LaseWat_2:
		UpdateGroupColorSet(
			uib->Idx(), 
			{this->btnLaseW_1, this->btnLaseW_2, this->btnLaseW_3}, 
			colSetButtonTog,
			colSetButton);

		SetLaserLevel(*this->GetCoreWindow()->hwLaser, 2);
		break;

	case UIID::LaseWat_3:
		UpdateGroupColorSet(
			uib->Idx(), 
			{this->btnLaseW_1, this->btnLaseW_2, this->btnLaseW_3}, 
			colSetButtonTog,
			colSetButton);

		SetLaserLevel(*this->GetCoreWindow()->hwLaser, 3);
		break;

	case UIID::Lase_Exposure_1:
		UpdateGroupColorSet(
			uib->Idx(), 
			{this->btnExp_1, this->btnExp_2, this->btnExp_3, this->btnExp_4}, 
			colSetButtonTog,
			colSetButton);

		this->GetView()->UseExposureSetting(0);
		break;

	case UIID::Lase_Exposure_2:
		UpdateGroupColorSet(
			uib->Idx(), 
			{this->btnExp_1, this->btnExp_2, this->btnExp_3, this->btnExp_4}, 
			colSetButtonTog,
			colSetButton);

		this->GetView()->UseExposureSetting(1);
		break;

	case UIID::Lase_Exposure_3:
		UpdateGroupColorSet(
			uib->Idx(), 
			{this->btnExp_1, this->btnExp_2, this->btnExp_3, this->btnExp_4}, 
			colSetButtonTog,
			colSetButton);

		this->GetView()->UseExposureSetting(2);
		break;

	case UIID::Lase_Exposure_4:
		UpdateGroupColorSet(
			uib->Idx(), 
			{this->btnExp_1, this->btnExp_2, this->btnExp_3, this->btnExp_4}, 
			colSetButtonTog,
			colSetButton);

		this->GetView()->UseExposureSetting(3);
		break;

	case UIID::Lase_Threshold_None:
		this->DoThresholdButton(UIID::Lase_Threshold_None, ProcessingType::None);
		break;

	case UIID::Lase_Threshold_Simple:
		this->DoThresholdButton(UIID::Lase_Threshold_Simple, ProcessingType::static_threshold);
		break;

	case UIID::Lase_Threshold_Mean2:
		this->DoThresholdButton(UIID::Lase_Threshold_Mean2, ProcessingType::two_stdev_from_mean);
		break;

	case UIID::Lase_Threshold_Yen:
		this->DoThresholdButton(UIID::Lase_Threshold_Yen, ProcessingType::yen_threshold);
		break;

	case UIID::Lase_Threshold_YenSimple:
		this->DoThresholdButton(UIID::Lase_Threshold_YenSimple, ProcessingType::yen_threshold_compressed);
		break;

	case UIID::CamSet_Exposure:
		this->ManageCamButtonPressed(uiId);
		break;

	case UIID::CamSet_Disparity:
		this->ManageCamButtonPressed(uiId);
		break;

	case UIID::CamSet_Opacity:
		this->ManageCamButtonPressed(uiId);
		break;

	case UIID::CamSet_Register:
		this->ManageCamButtonPressed(uiId);
		break;

	case UIID::CamSet_Calibrate:
		this->ManageCamButtonPressed(uiId);
		break;

	case UIID::CamSet_Threshold:
		this->ManageCamButtonPressed(uiId);
		break;

	case UIID::CamSet_Threshold_SlideThresh:
		break;

	case UIID::CamSet_Threshold_DispUp:
		break;

	case UIID::CamSet_Threshold_Back:
		this->camButtonGrid->Show();
		break;

	case UIID::CamSet_Opacity_Meter:
		break;

	case UIID::CamSet_Opacity_Back:
		break;

	case UIID::CamSet_Calibrate_Slider:
		break;

	case UIID::Exit_Confirm:
		this->GetCoreWindow()->ChangeState(BaseState::AppState::Exit);
	}
}

void StateHMDOp::OnUISink_ChangeValue(UIBase* uib, float value, int vid)
{
	int uiId = uib->Idx();
	switch(uiId)
	{
	case UIID::CamSet_Threshold_SlideThresh:
		{
			int targIdx = this->GetView()->cachedOptions.FindMenuTargetIndex();
			if(targIdx == -1)
				return;

			CamStreamMgr& cmgr = CamStreamMgr::GetInstance();
			cmgr.SetParam(targIdx, StreamParams::StaticThreshold, value);
		}
		break;

	case UIID::CamSet_Opacity_Meter:
		{
			int targIdx = this->GetView()->cachedOptions.FindMenuTargetIndex();
			if(targIdx == -1)
				return;

			CamStreamMgr& cmgr = CamStreamMgr::GetInstance();
			cmgr.SetParam(targIdx, StreamParams::Alpha, value);
		}
	}
}

void StateHMDOp::InjectIntoDicom(DcmDataset* dicomData)
{
	std::lock_guard<std::mutex> guard(this->dicomMutex);

	// TODO: Needs a bit more clarification on what carousel maps to what tag.
	std::string seriesCaption = this->caroSeries.GetCurrentCaption();
	dicomData->putAndInsertString(DCM_SeriesDescription, seriesCaption.c_str());
	dicomData->putAndInsertString(DCM_SeriesNumber, std::to_string(this->caroSeries.GetCurrentIndex()).c_str());

	std::string bodyCaption = this->caroBody.GetCurrentCaption();
	dicomData->putAndInsertString(DCM_BodyPartExamined, bodyCaption.c_str());

	// The Orientation/Aspect involved a bit more elbow grease, because Dicom
	// has specific names, which are a little different than the terms we're
	// supporting.
	//
	// These values are hard coded to the IDs of the default caroOrient values.
	//
	// See https://dicom.innolitics.com/ciods/mr-image/general-image/00200020
	// For the expected Dicom values.
	std::string orientID = this->caroOrient.GetCurrentID();
	std::string dicomOrient;
	if(orientID == "AN")
		dicomOrient = "A";
	else if(orientID == "SU")
		dicomOrient = "H";		// NOTE: Not correct - currently set to head - ask
	else if(orientID == "PO")
		dicomOrient = "P";
	else if(orientID == "IN")
		dicomOrient = "F";		// NOTE: Not correct - currently set to foot - ask
	else if(orientID == "LA")
		dicomOrient = "L";
	else if(orientID == "ME")
		dicomOrient = "M";
	
	if(dicomOrient.empty())
	{
		std::cerr << "Attempting to inject into Dicom an unknown aspect/orientation id " << orientID << ".";
	}
	else
	{
		dicomData->putAndInsertString(DCM_PatientOrientation, dicomOrient.c_str());
	}
}

void StateHMDOp::RefreshLaserBackPlate(bool expanded)
{
	// This should be pretty much the same place at the btnLaser
	const float lasVBackPad = 10.0f;
	const float lasHMargin = 2.0f;

	if(expanded)
	{
		this->plateMainLaserBack->SetLocPos(lasHMargin, -icoDim/2.0f - lasVBackPad);
		this->plateMainLaserBack->SetDim(maxVertWidth - 2 * lasHMargin, icoDim + lasVBackPad * 2.0f);
	}
	else
	{
		this->plateMainLaserBack->SetLocPos(lasHMargin, -icoDim/2.0f - lasVBackPad);
		this->plateMainLaserBack->SetDim(minVertWidth - 2 * lasHMargin, icoDim + lasVBackPad * 2.0f);
		
	}
}

void StateHMDOp::SetLaserBackPlateColor(bool toggled)
{
	const static UIColor4 colLaserPlateSel(0.5f,	0.25f,	0.25f);
	const static UIColor4 colLaserPlateUnsel(0.25f, 0.25f,	0.25f);

	if(toggled)
		this->plateMainLaserBack->uiCols.norm = colLaserPlateSel;
	else
		this->plateMainLaserBack->uiCols.norm = colLaserPlateUnsel;
}