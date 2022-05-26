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
	UIColor4(1.0f, 1.0f, 1.0f),	// White
	UIColor4(0.5f, 1.0f, 0.5f),	// Green
	UIColor4(0.7f, 1.0f, 0.7f));// Bright green

// The standard colors for buttons that are toggled on
ColorSetInteractable colSetButtonTog(
	UIColor4(1.0f, 0.2f, 0.2f),	// Red
	UIColor4(1.0f, 0.5f, 0.5f),	// Pink
	UIColor4(1.0f, 0.7f, 0.7f));// Bright pink

void SetButtonStdCols(UIBase* uib, bool toggled = false)
{
	if(!toggled)
		uib->uiCols = colSetButton;
	else
		uib->uiCols = colSetButtonTog;
}

void StateHMDOp::MouseDownState::Reset()
{
	this->sinceClick = 0.0f;
	this->isDown = false;
}

void StateHMDOp::MouseDownState::Decay(double dt)
{
	if(this->isDown)
		return;

	this->sinceClick = (float)std::max(0.0, this->sinceClick	- dt * clickDecayRate);
}

void StateHMDOp::MouseDownState::FlagUp()
{
	this->isDown = false;
}

void StateHMDOp::MouseDownState::FlagDown()
{
	this->isDown = true;
	this->sinceClick = 1.0f;
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

	this->texSliderSysPos = TexObj::MakeSharedLODE("Circle_Pos.png");
	this->texSliderSysNeg = TexObj::MakeSharedLODE("Circle_Neg.png");

	this->uiSys.SetSelectingButtons(true, true, true);

	this->patch_circle		= TexObj::MakeSharedLODE("8mmCircle.png"		);
	this->patch_roundLeft	= TexObj::MakeSharedLODE("8mmRoundedLeft.png"	);
	this->patch_roundRight	= TexObj::MakeSharedLODE("8mmRoundedRight.png"	);
	this->ninePatchCircle	= NinePatcher::MakeFromPixelsMiddle(this->patch_circle);

	this->patch_smallCircle	= TexObj::MakeSharedLODE("3mmCircle.png");
	this->ninePatchSmallCircle = NinePatcher::MakeFromPixelsMiddle(this->patch_smallCircle);

	
	const int btnTextSz = 14.0f;

	// Build elements for the right menu bar
	//
	this->vertMenuPlate = new UIPlate( &this->uiSys, UIID::PullMenu, UIRect(960, 540, 50.0f, 100.0f), plateGray);
	this->vertMenuPlate->SetMode_Patch(this->patch_roundRight, this->ninePatchCircle);
	//
	this->btnLaser		= new UIButton(this->vertMenuPlate, UIID::MBtnLaserTog, UIRect(20.0f, -icoDim/2.0f, icoDim, icoDim),	"Menu_Icon_Laser.png"	);
	this->btnLaser->SetPivot(0.0f, 0.5f - menuYPiv * 2.0f);
	SetButtonStdCols(this->btnLaser);
	this->btnSettings	= new UIButton(this->vertMenuPlate, UIID::MBtnLaserSet, UIRect(20.0f, -icoDim/2.0f, icoDim, icoDim),	"Menu_Icon_Sliders.png");
	this->btnSettings->SetPivot(0.0f, 0.5f - menuYPiv * 0.5f);
	SetButtonStdCols(this->btnSettings);
	this->btnAlign		= new UIButton(this->vertMenuPlate, UIID::MBtnAlign,	UIRect(20.0f, -icoDim/2.0f, icoDim, icoDim),	"Menu_Icon_Align.png"	);
	this->btnAlign->SetPivot(0.0f, 0.5f + menuYPiv * 0.5f);
	SetButtonStdCols(this->btnAlign);
	this->btnCamSets	= new UIButton(this->vertMenuPlate, UIID::MBtnSource,	UIRect(20.0f, -icoDim/2.0f, icoDim, icoDim),	"Menu_Icon_Return.png"	);
	this->btnCamSets->SetPivot(0.0f, 0.5f + menuYPiv * 1.5f);
	SetButtonStdCols(this->btnCamSets);
	this->btnBack		= new UIButton(this->vertMenuPlate, UIID::MBtnBack,		UIRect(20.0f, -icoDim/2.0f, icoDim, icoDim * 0.5f),	"Menu_Back.png"	)	;
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
	this->inspSetFrame->UseDyn()->AnchorsTop().SetOffsets(10.0f, titleHeight, -10.0f, 250.0f);
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
	this->btnThreshTy = new UIButton(inspSettingsPlate, UIID::Lase_ThresholdType, UIRect(20, 300, 100, 30), "THRESH:", btnTextSz);
	this->btnThreshTy->UseDyn()->SetAnchors(0.0f, 0.0f, 0.5f, 0.0f).SetOffsets(0.0f, 300.0f, 0.0f, 330.0f);
	this->ApplyFormButtonStyle(this->btnThreshTy);
	SetButtonStdCols(this->btnThreshTy, false);
	// TODO: Initialize threshold based on starting value.
	this->btnThreshTog = new UIButton(inspSettingsPlate, UIID::Lase_ThresholdToggle, UIRect(120, 300, 100, 30), "ON", btnTextSz);
	this->btnThreshTog->UseDyn()->SetAnchors(0.5f, 0.0f, 1.0f, 0.0f).SetOffsets(0.0f, 300.0f, 0.0f, 330.0f);
	this->ApplyFormButtonStyle(this->btnThreshTog);
	
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
		btnIt += BtnStride;
		this->camBtnThresh = new UIButton(this->camButtonGrid, UIID::CamSet_Threshold, UIRect(), "Threshold Settings", btnTextSz);
		this->ApplyFormButtonStyle(this->camBtnThresh);
		this->camBtnThresh->UseDyn()->AnchorsTop().SetOffsets(BtnHPad, btnIt, -BtnHPad, btnIt + BtnH);
		SetButtonStdCols(this->camBtnThresh);
		//btnIt += BtnStride;

		this->plateSliderCalibrate = this->CreateSliderSystem(&this->uiSys, UIID::CamSet_Calibrate_Slider,	"Calibrate", 0.0f, 1.0f, 0.25f, UIRect());
		this->horizontalFloatingUISys.push_back(this->plateSliderCalibrate);
	}
	{
		this->plateSliderOpacity = this->CreateSliderSystem(&this->uiSys, UIID::CamSet_Opacity_Meter, "Opacity", 0.0f, 1.0f, 0.25f, UIRect());
		this->horizontalFloatingUISys.push_back(this->plateSliderOpacity);
	}
	{	
		this->plateSliderThresh = this->CreateSliderSystem(&this->uiSys, UIID::CamSet_Threshold_SlideThresh,	"Thresh", 0.0f, 1.0f, 0.25f, UIRect());
		this->horizontalFloatingUISys.push_back(this->plateSliderThresh);
		this->plateSliderDispup = this->CreateSliderSystem(&this->uiSys, UIID::CamSet_Threshold_DispUp,		"DispUp", 0.0f, 1.0f, 0.25f, UIRect());
		this->horizontalFloatingUISys.push_back(this->plateSliderDispup);
	}
	this->ManageCamButtonPressed(-1, false);
	
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

void StateHMDOp::DrawMousePad(float x, float y, float scale, bool ldown, bool rdown, bool mdown)
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor3f(1.0f, 1.0f, 1.0f);

	// For all the quads we're about to lay down, we're starting at
	// the top left and moving clockwise.
	this->ico_MousePadCrevice.GLBind();
	float invLeftCl		= 1.0f - this->mdsLeft.sinceClick;
	float invRightCl	= 1.0f - this->mdsRight.sinceClick;
	float invMiddle		= 1.0f - this->mdsMiddle.sinceClick;
	//
	DrawOffsetVertices(x, y, this->ico_MousePadCrevice, 0.5f, 1.0f, scale);
	//
	glColor3f(invMiddle, 1.0f, invMiddle);
	DrawOffsetVertices(x, y, this->ico_MousePadBall,	0.5f, 0.5f, scale);
	//
	glColor3f(invLeftCl, 1.0f, invLeftCl);
	DrawOffsetVertices(x, y, this->ico_MousePadLeft,	1.0f, 1.0f, scale);
	//
	glColor3f(invRightCl, 1.0f, invRightCl);
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
	const int camCt = 2;
	for(int i = 0; i < camCt; ++i)
	{
		std::stringstream sstrm;
		sstrm << "Cam: " << i << " - MS: " << camMgr.GetMSFrameTime(i);
		this->fontInsTitle.RenderFont(sstrm.str().c_str(), 0, sz.y - (20 * camCt) + (20 * i));
	}

	this->vertMenuPlate->SetLocPos(cameraWindowRgn.EndX() + 10.0f, cameraWindowRgn.y + 25.0f);
	this->vertMenuPlate->SetDim(this->curVertWidth, cameraWindowRgn.h - 50.0f);

	this->uiSys.AlignSystem();
	this->uiSys.Render();
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

		this->plateSliderCalibrate->SetRect(r.LerpHoriz(lerpLeft),  r.Bottom() - 20.0f, r.LerpWidth(lerpWidth), rowHeight);
		//
		this->plateSliderOpacity->SetRect(r.LerpHoriz(lerpLeft),  r.Bottom() - 20.0f, r.LerpWidth(lerpWidth), rowHeight);
		//
		this->plateSliderThresh->SetRect(r.LerpHoriz(lerpLeft),  r.Bottom() - 20.0f, r.LerpWidth(lerpWidth), rowHeight);
		this->plateSliderDispup->SetRect(r.LerpHoriz(lerpLeft),  r.Bottom() - 20.0f + rowOffs, r.LerpWidth(lerpWidth), rowHeight);
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
	if(showVertMenu)
	{ 
		curVertWidth = (float)std::min<float>(maxVertWidth, curVertWidth + vertTransSpeed * dt);
		showRButtons = (curVertWidth == maxVertWidth);
	}
	else
		curVertWidth = (float)std::max<float>(minVertWidth, curVertWidth - vertTransSpeed * dt);

	this->btnLaser->Show(showRButtons);
	this->btnSettings->Show(showRButtons);
	this->btnAlign->Show(showRButtons);
	this->btnCamSets->Show(showRButtons);
	this->btnBack->Show(showRButtons);
}

void StateHMDOp::EnteredActive()
{
	// While these texture loads were initially in Initialize(),
	// they've been moved to EnteredActive to deffer them, because
	// there was some texture loading issues on the Linux side -
	//
	// most likely because the OpenGL context doesn't get initialized
	// as fast as on Windows.
	this->ico_MousePadLeft.LODEIfEmpty("Mousepad_Left.png");
	this->ico_MousePadRight.LODEIfEmpty("Mousepad_Right.png");
	this->ico_MousePadCrevice.LODEIfEmpty("Mousepad_Crevice.png");
	this->ico_MousePadBall.LODEIfEmpty("Mousepad_MiddleBall.png");

	this->mdsLeft.Reset();
	this->mdsMiddle.Reset();
	this->mdsRight.Reset();
} 

void StateHMDOp::ExitedActive() 
{
	this->camTextureRegistry.ClearTextures();
	this->uiSys.DelegateReset();
}

void StateHMDOp::Initialize() 
{
	this->fontInsTitle = FontMgr::GetInstance().GetFont(24);
}

void StateHMDOp::OnKeydown(wxKeyCode key)
{
	// The numpad items will need to check for two keys, because it
	// can vary based on if NumLock is on or not.
	//
	// DEV SHORTCUTS: Snap a picture
	if(key == WXK_NUMPAD7		|| key == WXK_NUMPAD_HOME)
		this->GetCoreWindow()->RequestSnap(0, "RGB");
	else if(key == WXK_NUMPAD8	|| key == WXK_NUMPAD_UP)
		this->GetCoreWindow()->RequestSnap(1, "NIR");
	//
	// DEV SHORTCUTS: START recording a camera to video 
	else if(key == WXK_NUMPAD4	|| key == WXK_NUMPAD_LEFT)
		this->GetCoreWindow()->RecordVideo(0, "RGB");
	else if(key == WXK_NUMPAD5	|| key == WXK_CLEAR)
		this->GetCoreWindow()->RecordVideo(1, "NIR");
	//
	// DEV SHORTCUTS: STOP recording a camera to video.
	else if(key == WXK_NUMPAD1	|| key == WXK_NUMPAD_END)
		this->GetCoreWindow()->StopRecording(0);
	else if(key == WXK_NUMPAD2	|| key == WXK_NUMPAD_DOWN)
		this->GetCoreWindow()->StopRecording(1);

	// Keyboard shortcut for the pull-out menu
	else if(key == WXK_NUMPAD_ENTER)
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
				// If nothing is shown and the left pedal is pressed, take a photo.
				this->GetCoreWindow()->RequestSnap(0, "snap");
			}
			else if(button == 1)
			{
				// If nothing is shown and the middle pedal is pressed, open the carousel.

			}
			else if(button == 2)
			{
				// If nothing is shown an the right pedal is pressed, start video recording
				this->GetCoreWindow()->RecordVideo(0, "video");
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

void StateHMDOp::SetShownMenuBarUIPanel(int idx)
{
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
	for(UIBase* uib : this->horizontalFloatingUISys)
		uib->Hide();

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

	switch(buttonID)
	{
		case CamSet_Exposure:
			SetButtonStdCols(this->camBtnExposure, true);
			break;
		case CamSet_Disparity:
			SetButtonStdCols(this->camBtnDisparity, true);
			break;
		case CamSet_Opacity:
			this->plateSliderOpacity->Show();
			SetButtonStdCols(this->camBtnOpacity, true);
			break;
		case CamSet_Register:
			SetButtonStdCols(this->camBtnRegisterXY, true);
			break;
		case CamSet_Calibrate:
			this->plateSliderCalibrate->Show();
			SetButtonStdCols(this->camBtnCalibrate, true);
			break;
		case CamSet_Threshold:
			this->plateSliderThresh->Show();
			this->plateSliderDispup->Show();
			SetButtonStdCols(this->camBtnThresh, true);
			break;
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
	const UIRect& r)
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

	return retPlate;
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

	case UIID::Lase_ThresholdType:
		break;

	case UIID::Lase_ThresholdToggle:
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