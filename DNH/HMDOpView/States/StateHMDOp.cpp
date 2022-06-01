#include "StateHMDOp.h"
#include "StateIncludes.h"
#include "../CamVideo/CamStreamMgr.h"
#include "../Utils/cvgShapes.h"
#include "../UISys/UIPlate.h"
#include "../UISys/UIButton.h"
#include "../UISys/UIVBulkSlider.h"
#include <cmath>

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

void StateHMDOp::MouseDownState::Reset()
{
	this->clickRecent = 0.0f;
	this->isDown = false;
}

void StateHMDOp::MouseDownState::Decay(double dt)
{
	if(this->isDown)
		return;

	this->clickRecent = (float)std::max(0.0, this->clickRecent	- dt * clickDecayRate);
}

void StateHMDOp::MouseDownState::FlagUp()
{
	this->isDown = false;
}

void StateHMDOp::MouseDownState::FlagDown()
{
	this->isDown = true;
	this->clickRecent = 1.0f;
}

const float icoDim = 60.0f;					// (Square) Size of main menu icons/buttons in pixels
const float menuYPiv = 1.05f / 7.0f;		// Separation of main menu icons, in terms of percentage of the main menu bar.

const UIColor4 plateGray(0.5f, 0.5f, 0.5f, 1.0f);
const UIColor4 menuTitleCol(0.0f, 0.0f, 0.0f, 1.0f);
const float titleHeight = 40.0f;

StateHMDOp::StateHMDOp(HMDOpApp* app, GLWin* view, MainWin* core)
	:	BaseState(BaseState::AppState::MainOp, app, view, core),
		uiSys(-1, UIRect(0, 0, 1920, 1080), this)
{
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

	// Build elements for the right menu bar
	//
	this->vertMenuPlate = new UIPlate( &this->uiSys, UIID::PullMenu, UIRect(960, 540, 50.0f, 100.0f), plateGray);
	this->vertMenuPlate->SetMode_Patch(this->patch_roundRight, this->ninePatchCircle);
	//
	this->btnLaser		= new UIButton(this->vertMenuPlate, UIID::MBtnLaserTog, UIRect(20.0f, -icoDim/2.0f, icoDim, icoDim),		"Assets/MenubarIcos/Menu_Icon_Laser.png"	);
	this->btnLaser->SetPivot(0.0f, 0.5f - menuYPiv * 2.0f);
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
	this->btnBack		= new UIButton(this->vertMenuPlate, UIID::MBtnBack,		UIRect(20.0f, -icoDim/2.0f, icoDim, icoDim * 0.5f),	"Assets/MenubarIcos/Menu_Back.png"	)	;
	this->btnBack->SetPivot(0.0f, 0.5f + menuYPiv * 3.15f);
	SetButtonStdCols(this->btnBack);

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
	UIText* textLaseWatts = new UIText(this->inspSetFrame, -1, "LASE WATTS", 20, UIRect());
	textLaseWatts->UseDyn()->AnchorsTop().SetOffsets(0.0f, 0.0f, 0.0f, 40.0f);
	textLaseWatts->uiCols.norm.SetColor_Black();
	this->btnLaseW_1 = new UIButton(this->inspSetFrame, UIID::LaseWat_1, UIRect(),	"L1", btnTextSz);
	this->btnLaseW_1->UseDyn()->SetAnchors(0.0f, 0.0f, 0.33f, 0.0f).SetOffsets(0.0f, 40.0f, 0.0f, 70.0f);
	this->ApplyFormButtonStyle(this->btnLaseW_1);
	SetButtonStdCols(this->btnLaseW_1, true);
	this->btnLaseW_2 = new UIButton(this->inspSetFrame, UIID::LaseWat_2, UIRect(),	"L2", btnTextSz);
	this->btnLaseW_2->UseDyn()->SetAnchors(0.33f, 0.0f, 0.66f, 0.0f).SetOffsets(0.0f, 40.0f, 0.0f, 70.0f);
	this->ApplyFormButtonStyle(this->btnLaseW_2);
	SetButtonStdCols(this->btnLaseW_2, false);
	this->btnLaseW_3 = new UIButton(this->inspSetFrame, UIID::LaseWat_3, UIRect(),	"L3", btnTextSz);
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
	
	//
	this->inspAlignPlate	= new UIPlate( &this->uiSys, -1, defInspPlateDim, plateGray);
	this->inspAlignPlate->SetMode_Patch(this->patch_roundLeft, this->ninePatchCircle);
	this->inspAlignPlate->Show(false);
	//
	UIText* alignTitle = new UIText(this->inspAlignPlate, -1, "Reregister", 20, UIRect());
	alignTitle->UseDyn()->AnchorsTop().SetOffsets(0.0f, 0.0f, 0.0f, titleHeight);
	alignTitle->uiCols.SetAll(menuTitleCol);

	//
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

		const float BtnHPad = 10.0f;
		const float BtnVPad = 10.0f;
		const float BtnH = 40.0f;
		const float BtnStride = BtnH + BtnVPad;
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
			this->sliderSysOpacity = this->CreateSliderSystem(&this->uiSys, UIID::CamSet_Opacity_Meter, "Opacity", 0.0f, 1.0f, 0.25f, UIRect(), this->camBtnOpacity);
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
	
	this->ManageCamButtonPressed(-1, false);
	
}

void StateHMDOp::AddCamGroupingEntry(UIButton* button, UIPlate* system, UIHSlider* slider)
{
	this->AddCamGroupingEntry(button, {system, slider});
}

void StateHMDOp::AddCamGroupingEntry(UIButton* button, PlateSliderPair pair)
{
	this->camButtonGrouping[button].push_back(pair);
}

void DrawOffsetVertices(
	float x, 
	float y, 
	float w, 
	float h, 
	float px, 
	float py, 
	float scale)
{
	float toLeft	= -px *			w * scale;
	float toRight	= (1.0f - px) * w * scale;
	float toTop		= -py *			h * scale;
	float toBot		= (1.0 - py) *	h * scale;

	glBegin(GL_QUADS);
	{
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(x + toLeft, y + toTop);
		//
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(x + toRight, y + toTop);
		//
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(x + toRight, y + toBot);
		//
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(x + toLeft, y + toBot);
	}
	glEnd();
}

void DrawOffsetVertices(float x, float y, TexObj& to, float px, float py, float scale)
{
	to.GLBind();
	DrawOffsetVertices(x, y, to.width, to.height, px, py, scale);
}

UIColor4 GetMousepadColor(bool isDown, float timeSinceClick)
{
	// Pressed down
	if(isDown)
		return UIColor4(0.75f, 0.75f, 0.25f, 1.0f);

	// Fading
	float colorComp =  std::clamp(1.0f - timeSinceClick, 0.0f, 1.0f);
	return UIColor4(colorComp, 1.0f, colorComp);
}

UIColor4 GetMousepadColor(float timeSinceClick)
{
	return GetMousepadColor((1.0f - timeSinceClick) == 0.0f, timeSinceClick);
}

void StateHMDOp::DrawMousePad(float x, float y, float scale, bool ldown, bool rdown, bool mdown)
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor3f(1.0f, 1.0f, 1.0f);

	// For all the quads we're about to lay down, we're starting at
	// the top left and moving clockwise.
	this->ico_MousePadCrevice.GLBind();
	UIColor4 colMLeft	= GetMousepadColor(this->mdsLeft.clickRecent		);
	UIColor4 colMMiddle = GetMousepadColor(this->mdsMiddle.clickRecent	);
	UIColor4 colMRight	= GetMousepadColor(this->mdsRight.clickRecent	);
	
	//
	DrawOffsetVertices(x, y, this->ico_MousePadCrevice, 0.5f, 1.0f, scale);
	//
	colMMiddle.GLColor4();
	DrawOffsetVertices(x, y, this->ico_MousePadBall,	0.5f, 0.5f, scale);
	//
	colMLeft.GLColor4();
	DrawOffsetVertices(x, y, this->ico_MousePadLeft,	1.0f, 1.0f, scale);
	//
	colMRight.GLColor4();
	DrawOffsetVertices(x, y, this->ico_MousePadRight,	0.0f, 1.0f, scale);
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

		if(camMgr.IsThresholded(camIt))
			glColor4f(0.5f, 0.0f, 0.0f, 1.0f);
		else
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		glBegin(GL_QUADS);
			viewRegion.GLVerts_Textured();
		glEnd();
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
	
	this->DrawMenuSystemAroundRect(cameraWindowRgn);

	this->DrawMousePad(
		sz.x /2 + (float)this->GetView()->mousepadOffsX,		// Horizontally at the center
		sz.y / 2 + (float)this->GetView()->mousepadOffsY,		// Near the bottom
		(float)this->GetView()->mousepadScale, 
		false, 
		false, 
		false);

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

	this->uiSys.AlignSystem();
	this->uiSys.Render();

	if(this->showCarousel)
		this->carousel.Render(sz.x * 0.5f, 850.0f, this->carouselStyle, 1.0f);
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
	if(this->inspectorShow == true)
	{
		// TODO: Slide in/out inspector
	}

	this->mdsLeft.Decay(dt);
	this->mdsMiddle.Decay(dt);
	this->mdsRight.Decay(dt);

	bool showRButtons = false;
	if(this->showVertMenu)
	{ 
		curVertWidth = (float)std::min<float>(maxVertWidth, curVertWidth + vertTransSpeed * dt);
		showRButtons = (curVertWidth == maxVertWidth);

		this->HideCarousel();
	}
	else
		this->curVertWidth = (float)std::max<float>(minVertWidth, curVertWidth - vertTransSpeed * dt);

	this->btnLaser->Show(showRButtons);
	this->btnSettings->Show(showRButtons);
	this->btnAlign->Show(showRButtons);
	this->btnCamSets->Show(showRButtons);
	this->btnBack->Show(showRButtons);

	if(this->showCarousel)
		this->carousel.Update(this->carouselStyle, dt);
}

void StateHMDOp::EnteredActive()
{
	// While these texture loads were initially in Initialize(),
	// they've been moved to EnteredActive to deffer them, because
	// there was some texture loading issues on the Linux side -
	//
	// most likely because the OpenGL context doesn't get initialized
	// as fast as on Windows.
	this->ico_MousePadLeft.LODEIfEmpty(		"Assets/Mousepad/Mousepad_Left.png");
	this->ico_MousePadRight.LODEIfEmpty(	"Assets/Mousepad/Mousepad_Right.png");
	this->ico_MousePadCrevice.LODEIfEmpty(	"Assets/Mousepad/Mousepad_Crevice.png");
	this->ico_MousePadBall.LODEIfEmpty(		"Assets/Mousepad/Mousepad_MiddleBall.png");

	this->mdsLeft.Reset();
	this->mdsMiddle.Reset();
	this->mdsRight.Reset();

	this->_SyncImageProcessingSetUI();
	this->_SyncThresholdSlider();

	std::string curCapt = this->carousel.GetCurrentCaption();
	CamStreamMgr::GetInstance().SetAllSnapCaption(curCapt);
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
	this->sliderSysThresh.slider->SetCurValue(cmgr.GetFloat(targIdx, StreamParams::StaticThreshold));
}

void StateHMDOp::ExitedActive() 
{
	this->camTextureRegistry.ClearTextures();
	this->uiSys.DelegateReset();
}

void StateHMDOp::Initialize() 
{
	this->fontInsTitle = FontMgr::GetInstance().GetFont(24);

	this->carousel.Append(this->GetView()->cachedOptions.carouselEntries);
	this->carousel.LoadAssets();
}

void StateHMDOp::OnKeydown(wxKeyCode key)
{
	// Keyboard shortcut for the pull-out menu
	if(key == WXK_NUMPAD_ENTER)
		this->showVertMenu = !this->showVertMenu;
	else
	{
		// If not handled by an element outside the UI system,
		// delegate to the UI system and see if anything is interested
		// in handling this.
		this->uiSys.DelegateKeydown(key);
	}
}

void StateHMDOp::OnKeyup(wxKeyCode key)
{
	this->uiSys.DelegateKeyup(key);
}

void StateHMDOp::OnMouseDown(int button, const wxPoint& pt)
{
	if(button == 0)
		this->mdsLeft.FlagDown();
	else if(button == 1)
		this->mdsMiddle.FlagDown();
	else if(button == 2)
		this->mdsRight.FlagDown();

	DelMouseRet dmr = this->uiSys.DelegateMouseDown(button, UIVec2(pt.x, pt.y));

	if(dmr.evt == DelMouseRet::Event::MissedDown)
	{
		if(!this->AnyUIUp())
		{
			if(button == 0)
			{
				if (this->IsCarouselShown())
				{
					this->carousel.GotoPrev();
					CamStreamMgr::GetInstance().SetAllSnapCaption(this->carousel.GetCurrentCaption());
				}
				else
				{
					// If nothing is shown and the left pedal is pressed, take a photo from
					// all video streams.
					this->GetCoreWindow()->RequestSnapAll(this->carousel.GetCurrentLabel());
				}
			}
			else if(button == 2)
			{
				if(this->IsCarouselShown())
				{ 
					this->carousel.GotoNext();
					CamStreamMgr::GetInstance().SetAllSnapCaption(this->carousel.GetCurrentCaption());
				}
				else
				{
					// If nothing is shown an the right pedal is pressed, start video recording
					// of the composite stream

					// TODO: Record from composite stream instead of index 0
					if(this->GetCoreWindow()->IsRecording(SpecialCams::Composite))
						this->GetCoreWindow()->StopRecording(SpecialCams::Composite);
					else
						this->GetCoreWindow()->RecordVideo(SpecialCams::Composite, "video");
				}
			}
		}
	}
	if(	dmr.evt == DelMouseRet::Event::MissedDown || 
		dmr.evt == DelMouseRet::Event::MouseWhiffDown )
	{
		if(button == 1)
		{
			if(this->uiSys.IsUsingCustomTabOrder())
			{
				// Currently do do nothing, middle click whiff is handled 
				// in the UISink portion - it's not a clean implementation,
				// but regardless, it's the current authority on custom
				// tab navigation.
			}
			else if(
				this->inspSettingsPlate->IsSelfVisible() || 
				this->inspAlignPlate->IsSelfVisible() || 
				this->inspCamSetsPlate->IsSelfVisible())
			{
				// Else, if any of the inspector windows are open, middle mouse
				// will close the window.
				SetShownMenuBarUIPanel(-1);
			}
			else if(this->showVertMenu)
			{
				// If no submenu but the main menu is open, middle mouse will
				// close that window.
				this->uiSys.SubmitClick(this->btnBack, 2, UIVec2(), false);
			}
			else
			{
				// If nothing is shown and the middle pedal is pressed, 
				// toggle the carousel.
				this->ToggleCarousel();
			}

		}
	}
}

void StateHMDOp::OnMouseUp(int button, const wxPoint& pt)
{
	if(button == 0)
		this->mdsLeft.FlagUp();
	else if(button == 1)
	{
		this->showVertMenu = false;
		this->mdsMiddle.FlagUp();
	}
	else if(button == 2)
		this->mdsRight.FlagUp();

	this->uiSys.DelegateMouseUp(button, UIVec2(pt.x, pt.y));
}

/// <summary>
/// Query if a widget ID used in the StateHMDOp is in the main
/// right menu bar.
/// </summary>
/// <param name="id">The ID of the UI item.</param>
/// <returns>True if a part of the menu bar.</returns>
bool IsRightMenuItem(int id)
{
	switch(id)
	{
	case StateHMDOp::PullMenu:
	case StateHMDOp::MBtnLaserTog:
	case StateHMDOp::MBtnLaserSet:
	case StateHMDOp::MBtnAlign:
	case StateHMDOp::MBtnSource:
	case StateHMDOp::MBtnBack:
		return true;
	}
	return false;
}

void StateHMDOp::OnMouseMove(const wxPoint& pt)
{
	DelMouseRet m = this->uiSys.DelegateMouseMove(UIVec2(pt.x, pt.y));

	if(m.evt == DelMouseRet::Event::Hovered)
	{
		if(IsRightMenuItem(m.idx))
			this->showVertMenu = true;
	}	
}

void StateHMDOp::ClosingApp() 
{
}

StateHMDOp::~StateHMDOp()
{
}

bool StateHMDOp::ShowCarousel( bool show)
{

	if(this->showCarousel == show)
		return false;

	this->showCarousel = show;
	this->carousel.EndAnimation(this->carouselStyle, true);

	return true;
}

bool StateHMDOp::HideCarousel()
{
	return this->ShowCarousel(false);
}

bool StateHMDOp::ToggleCarousel()
{
	if(this->showCarousel)
		return this->HideCarousel();
	else
	{ 
		this->uiSys.ClearCustomTabOrder();
		return this->ShowCarousel();
	}
}

void StateHMDOp::SetShownMenuBarUIPanel(int idx)
{
	// Regardless of why this is called, a custom tab navigation
	// should not be setup when first entering into a panel. If we
	// want to cycle through the custom sets, a middle whiff needs
	// to occur.
	this->uiSys.ClearCustomTabOrder();

	bool toggleOff = false;
	// Check if the thing is already on, if so, it's a complete toggle off...

	switch(idx)
	{
	case UIID::MBtnLaserSet:
		if(this->inspSettingsPlate->IsSelfVisible())
			toggleOff = true;
		break;

	case UIID::MBtnAlign:
		if(this->inspAlignPlate->IsSelfVisible())
			toggleOff = true;
		break;

	case UIID::MBtnSource:
		if(this->inspCamSetsPlate->IsSelfVisible())
			toggleOff = true;
		break;
	}

	// TODO: when turning off, we eventually need to handle a slide-in 
	// animation for the current active plate being disabled.

	// Regardless of toggling everything off, or toggling everything off
	// except the new context, we're going to turn it all off.
	this->inspSettingsPlate->Hide();
	this->inspCamSetsPlate->Hide();
	this->inspAlignPlate->Hide();

	if(toggleOff)
	{
		this->ManageCamButtonPressed(-1, false);

		UpdateGroupColorSet( 
			-1,
			{this->btnSettings, this->btnAlign, this->btnCamSets},
			colSetButtonTog,
			colSetButton);

		return;
	}

	// ... Else, it's a toggle on, so we turn off everything except for the one
	// thing we've switched our UI context to.

	UpdateGroupColorSet( 
		idx,
		{this->btnSettings, this->btnAlign, this->btnCamSets},
		colSetButtonTog,
		colSetButton);

	switch(idx)
	{
	case UIID::MBtnLaserSet:
		this->inspSettingsPlate->Show();
		this->ManageCamButtonPressed(-1, false); // Turn all cam stuff off
		break;

	case UIID::MBtnAlign:
		this->inspAlignPlate->Show();
		this->ManageCamButtonPressed(-1, false); // Turn all cam stuff off
		break;

	case UIID::MBtnSource:
		this->inspCamSetsPlate->Show();
		this->ManageCamButtonPressed(this->lastCamButtonSel, false); // Restore last state
		break;
	}
}

void StateHMDOp::ApplyFormButtonStyle(UIGraphic* uib)
{
	uib->SetMode_Patch(
		this->patch_smallCircle, 
		this->ninePatchSmallCircle);
}

void StateHMDOp::ManageCamButtonPressed(int buttonID, bool record)
{
	if(record)
		this->lastCamButtonSel = buttonID;

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

bool StateHMDOp::AnyUIUp()
{
	// TODO: Used to check if mouse buttons perform captures
	return false;
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
				this->GetCoreWindow()->hwLaser->SetNIRIntensity(1.0f);
				SetButtonStdCols(this->btnLaser, true);
			}
			else
			{ 
				this->GetCoreWindow()->hwLaser->SetNIRIntensity(0.0f);
				SetButtonStdCols(this->btnLaser, false);
			}
		}
		break;

	case UIID::MBtnLaserSet:
		this->SetShownMenuBarUIPanel(uib->Idx());
		break;

	case UIID::MBtnAlign:
		this->SetShownMenuBarUIPanel(uib->Idx());
		break;

	case UIID::MBtnSource:
		this->SetShownMenuBarUIPanel(uib->Idx());
		break;

	case UIID::MBtnBack:
		this->showVertMenu = false;
		this->curVertWidth = this->minVertWidth;
		this->SetShownMenuBarUIPanel(-1);
		this->ManageCamButtonPressed(-1, false);
		this->uiSys.ClearCustomTabOrder();
		this->uiSys.Select(nullptr);
		break;

	case UIID::LaseWat_1:
		UpdateGroupColorSet(
			uib->Idx(), 
			{this->btnLaseW_1, this->btnLaseW_2, this->btnLaseW_3}, 
			colSetButtonTog,
			colSetButton);

		// TODO: Placeholder
		this->GetCoreWindow()->hwLaser->SetWhiteIntensity(0.3f);
		break;

	case UIID::LaseWat_2:
		UpdateGroupColorSet(
			uib->Idx(), 
			{this->btnLaseW_1, this->btnLaseW_2, this->btnLaseW_3}, 
			colSetButtonTog,
			colSetButton);

		// TODO: Placeholder
		this->GetCoreWindow()->hwLaser->SetWhiteIntensity(0.6f);
		break;

	case UIID::LaseWat_3:
		UpdateGroupColorSet(
			uib->Idx(), 
			{this->btnLaseW_1, this->btnLaseW_2, this->btnLaseW_3}, 
			colSetButtonTog,
			colSetButton);

		// TODO: Placeholder
		this->GetCoreWindow()->hwLaser->SetWhiteIntensity(1.0f);
		break;

	case UIID::Lase_Exposure_1:
		UpdateGroupColorSet(
			uib->Idx(), 
			{this->btnExp_1, this->btnExp_2, this->btnExp_3, this->btnExp_4}, 
			colSetButtonTog,
			colSetButton);
		break;

	case UIID::Lase_Exposure_2:
		UpdateGroupColorSet(
			uib->Idx(), 
			{this->btnExp_1, this->btnExp_2, this->btnExp_3, this->btnExp_4}, 
			colSetButtonTog,
			colSetButton);
		break;

	case UIID::Lase_Exposure_3:
		UpdateGroupColorSet(
			uib->Idx(), 
			{this->btnExp_1, this->btnExp_2, this->btnExp_3, this->btnExp_4}, 
			colSetButtonTog,
			colSetButton);
		break;

	case UIID::Lase_Exposure_4:
		UpdateGroupColorSet(
			uib->Idx(), 
			{this->btnExp_1, this->btnExp_2, this->btnExp_3, this->btnExp_4}, 
			colSetButtonTog,
			colSetButton);
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
		this->ManageCamButtonPressed(uiId, true);
		break;

	case UIID::CamSet_Disparity:
		this->ManageCamButtonPressed(uiId, true);
		break;

	case UIID::CamSet_Opacity:
		this->ManageCamButtonPressed(uiId, true);
		break;

	case UIID::CamSet_Register:
		this->ManageCamButtonPressed(uiId, true);
		break;

	case UIID::CamSet_Calibrate:
		this->ManageCamButtonPressed(uiId, true);
		break;

	case UIID::CamSet_Threshold:
		this->ManageCamButtonPressed(uiId, true);
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

	}
}

void StateHMDOp::OnUISink_SelMouseDownWhiff(UIBase* uib, int mouseBtn)
{
	// There's a TON of duplicate logic going on that can be mechanized
	// and automated.

	// When left whiffing when the button is select, that will toggle
	// moving to the next UI element.
	//
	// For now we do this by hand, ManageCamButtonPressed() should have
	// everything we need to simulate button pressing and changing the
	// context to the new button.
	int uiId = uib->Idx();
	switch(uiId)
	{
	case UIID::MBtnLaserTog:
		if(mouseBtn == 0)
		{
			this->uiSys.SubmitClick(this->btnLaser, 2, UIVec2(), false);
		}
		else if(mouseBtn == 2)
		{
			this->uiSys.SubmitClick(this->btnSettings, 2, UIVec2(), true);
		}
		break;

	case UIID::MBtnLaserSet:
		if(mouseBtn == 0)
		{ 
			this->uiSys.SubmitClick(this->btnSettings, 2, UIVec2(), false);
		}
		else if(mouseBtn == 1)
		{
			this->uiSys.Select(this->btnLaseW_1);
			this->uiSys.SetCustomTabOrder(
				{
					this->btnLaseW_1, 
					this->btnLaseW_2, 
					this->btnLaseW_3
				});
		}
		else if(mouseBtn == 2)
		{
			this->uiSys.SubmitClick(this->btnAlign, 2, UIVec2(), true);
		}
		break;

	case UIID::MBtnAlign:
		if(mouseBtn == 0)
		{ 
			this->uiSys.SubmitClick(this->btnAlign, 2, UIVec2(), false);
		}
		else if(mouseBtn == 1)
		{
		}
		else if(mouseBtn == 2)
		{
			this->uiSys.SubmitClick(this->btnCamSets, 2, UIVec2(), true);
		}
		break;

	case UIID::MBtnSource:
		if(mouseBtn == 0)
		{ 
			this->uiSys.SubmitClick(this->btnCamSets, 2, UIVec2(), true);
		}
		else if(mouseBtn == 1)
		{
			this->uiSys.Select(this->camBtnExposure);
			this->uiSys.SetCustomTabOrder(
				{
					this->camBtnExposure, 
					this->camBtnDisparity, 
					this->camBtnOpacity,
					this->camBtnRegisterXY,
					this->camBtnCalibrate,
					this->camBtnThresh
				});
		}
		else if(mouseBtn == 2)
		{
			this->uiSys.Select(this->btnBack);
			this->SetShownMenuBarUIPanel(-1);
		}
		break;

	case UIID::MBtnBack:
		if(mouseBtn == 0)
		{
			this->uiSys.SubmitClick(this->btnBack, 2, UIVec2(), false);
		}
		else if(mouseBtn == 1)
		{
		}
		else if(mouseBtn == 2)
		{
			this->uiSys.Select(this->btnLaser);
		}
		break;

	case UIID::LaseWat_1:
	case UIID::LaseWat_2:
	case UIID::LaseWat_3:
		if(mouseBtn == 0)
		{
			this->uiSys.SubmitClick(uib, 2, UIVec2(), false);
		}
		else if(mouseBtn == 1)
		{
			this->uiSys.AdvanceTabbingOrder(true);
		}
		else //if(mouseBtn == 2)
		{
			this->uiSys.Select(this->btnExp_1);
			this->uiSys.SetCustomTabOrder(
				{
					this->btnExp_1, 
					this->btnExp_2, 
					this->btnExp_3, 
					this->btnExp_4
				});
		}
		break;

	case UIID::Lase_Exposure_1:
	case UIID::Lase_Exposure_2:
	case UIID::Lase_Exposure_3:
	case UIID::Lase_Exposure_4:
		if(mouseBtn == 0)
		{
			this->uiSys.SubmitClick(uib, 2, UIVec2(), false);
		}
		else if(mouseBtn == 1)
		{
			this->uiSys.AdvanceTabbingOrder(true);
		}
		else //if(mouseBtn == 2)
		{
			this->uiSys.Select(this->btnThreshSel_None);
			this->uiSys.SetCustomTabOrder(
				{
					this->btnThreshSel_None, 
					this->btnThreshTog_Simple, 
					this->btnThreshTog_Mean2, 
					this->btnThreshTog_Yen,
					this->btnThreshTog_YenSimple
				});
		}
		break;

	case UIID::Lase_Threshold_None:
	case UIID::Lase_Threshold_Simple:
	case UIID::Lase_Threshold_Mean2:
	case UIID::Lase_Threshold_Yen:
	case UIID::Lase_Threshold_YenSimple:
		if(mouseBtn == 0)
		{
			this->uiSys.SubmitClick(uib, 2, UIVec2(), false);
		}
		else if(mouseBtn == 1)
		{
			this->uiSys.AdvanceTabbingOrder(true);
		}
		else //if(mouseBtn == 2)
		{
			this->uiSys.Select(this->btnLaseW_1);
			this->uiSys.SetCustomTabOrder(
				{
					this->btnLaseW_1, 
					this->btnLaseW_2, 
					this->btnLaseW_3
				});
		}
		break;

	case UIID::CamSet_Exposure:
		if(mouseBtn == 0)
			this->uiSys.SubmitClick(uib, 2, UIVec2(), false);
		else if(mouseBtn == 1)
			this->uiSys.AdvanceTabbingOrder(true);
		else if(mouseBtn == 2)
			this->ManageCamButtonPressed(-1, false);
		break;

	case UIID::CamSet_Disparity:
		if(mouseBtn == 0)
			this->uiSys.SubmitClick(uib, 2, UIVec2(), false);
		else if(mouseBtn == 1)
			this->uiSys.AdvanceTabbingOrder(true);
		else if(mouseBtn == 2)
			this->ManageCamButtonPressed(-1, false);
		break;

	case UIID::CamSet_Opacity:
		if(mouseBtn == 0)
			this->uiSys.SubmitClick(uib, 2, UIVec2(), false);
		else if(mouseBtn == 1)
			this->uiSys.AdvanceTabbingOrder(true);
		else if(mouseBtn == 2)
			this->ManageCamButtonPressed(-1, false);
		break;

	case UIID::CamSet_Register:
		if(mouseBtn == 0)
			this->uiSys.SubmitClick(uib, 2, UIVec2(), false);
		else if(mouseBtn == 1)
			this->uiSys.AdvanceTabbingOrder(true);
		else if(mouseBtn == 2)
			this->ManageCamButtonPressed(-1, false);
		break;

	case UIID::CamSet_Calibrate:
		if(mouseBtn == 0)
			this->uiSys.SubmitClick(uib, 2, UIVec2(), false);
		else if(mouseBtn == 1)
			this->uiSys.AdvanceTabbingOrder(true);
		else if(mouseBtn == 2)
			this->ManageCamButtonPressed(-1, false);
		break;

	case UIID::CamSet_Threshold:
		if(mouseBtn == 0)
			this->uiSys.SubmitClick(uib, 2, UIVec2(), false);
		else if(mouseBtn == 1)
			this->uiSys.AdvanceTabbingOrder(true);
		else if(mouseBtn == 2)
			this->ManageCamButtonPressed(-1, false);
		break;

	case UIID::CamSet_Threshold_SlideThresh:
	case UIID::CamSet_Calibrate_Slider:
	case UIID::CamSet_Opacity_Meter:
	case UIID::CamSet_Threshold_DispUp:
		// For the sliders, left and right click are 
		// taken up by the UIHSlider whiff handlers.
		if(mouseBtn == 1)
		{
			this->uiSys.AdvanceTabbingOrder(true);
		}
		break;
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
			cmgr.SetFloat(targIdx, StreamParams::StaticThreshold, value);
		}
		break;
	}
}