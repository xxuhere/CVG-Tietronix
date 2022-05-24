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

const float icoDim = 80.0f;

StateHMDOp::StateHMDOp(HMDOpApp* app, GLWin* view, MainWin* core)
	:	BaseState(BaseState::AppState::MainOp, app, view, core),
		uiSys(-1, UIRect(0, 0, 1920, 1080), this)
{
	this->patch_circle		= TexObj::MakeSharedLODE("8mmCircle.png"		);
	this->patch_roundLeft	= TexObj::MakeSharedLODE("8mmRoundedLeft.png"	);
	this->patch_roundRight	= TexObj::MakeSharedLODE("8mmRoundedRight.png"	);
	this->ninePatchCircle	= NinePatcher::MakeFromPixelsMiddle(this->patch_circle);

	this->patch_smallCircle	= TexObj::MakeSharedLODE("3mmCircle.png");
	this->ninePatchSmallCircle = NinePatcher::MakeFromPixelsMiddle(this->patch_smallCircle);

	const UIColor4 plateGray(0.5f, 0.5f, 0.5f, 1.0f);
	const int btnTextSz = 14.0f;

	// Build elements for the right menu bar
	//
	this->vertMenuPlate = new UIPlate( &this->uiSys, UIID::PullMenu, UIRect(960, 540, 50.0f, 100.0f), plateGray);
	this->vertMenuPlate->SetMode_Patch(this->patch_roundRight, this->ninePatchCircle);
	//
	this->btnLaser		= new UIButton(this->vertMenuPlate, UIID::MBtnLaserTog, UIRect(20.0f, -icoDim/2.0f, icoDim, icoDim),	"Menu_Icon_Laser.png"	);
	this->btnLaser->SetPivot(0.0f, 1.0f/5.0f);
	SetButtonStdCols(this->btnLaser);
	this->btnSettings	= new UIButton(this->vertMenuPlate, UIID::MBtnLaserSet, UIRect(20.0f, -icoDim/2.0f, icoDim, icoDim),	"Menu_Icon_Sliders.png");
	this->btnSettings->SetPivot(0.0f, 2.0f/5.0f);
	SetButtonStdCols(this->btnSettings);
	this->btnAlign		= new UIButton(this->vertMenuPlate, UIID::MBtnAlign,	UIRect(20.0f, -icoDim/2.0f, icoDim, icoDim),	"Menu_Icon_Align.png"	);
	this->btnAlign->SetPivot(0.0f, 3.0f/5.0f);
	SetButtonStdCols(this->btnAlign);
	this->btnCamSets	= new UIButton(this->vertMenuPlate, UIID::MBtnSource,	UIRect(20.0f, -icoDim/2.0f, icoDim, icoDim),	"Menu_Icon_Return.png"	);
	this->btnCamSets->SetPivot(0.0f, 4.0f/5.0f);
	SetButtonStdCols(this->btnCamSets);

	UIRect defInspPlateDim(800, 200, 400, 600);
	// Build elements for the inspector
	//
	this->inspSettingsPlate = new UIPlate( &this->uiSys, -1, defInspPlateDim, plateGray);
	inspSettingsPlate->SetMode_Patch(this->patch_roundLeft, this->ninePatchCircle);
	this->inspSettingsPlate->Show(false);
	this->inspSetFrame = new UIPlate(this->inspSettingsPlate, -1, defInspPlateDim.DilateAtOrigin(-20.0f).SS_Height(250.0f));
	// TODO:
	// this->inspSetFrame->filled = false;
	this->inspSetFrame->SetAllColors(plateGray);
	UIText* textLaseWatts = new UIText(this->inspSetFrame, -1, "LASE WATTS", 20, UIRect(0.0f, 40.0f, 380, 20.0f));
	textLaseWatts->uiCols.norm.SetColor_Black();
	this->btnLaseW_1 = new UIButton(this->inspSetFrame, UIID::LaseWat_1, UIRect(20, 50, 50, 30),	"L1", btnTextSz);
	this->ApplyFormButtonStyle(this->btnLaseW_1);
	SetButtonStdCols(this->btnLaseW_1, true);
	this->btnLaseW_2 = new UIButton(this->inspSetFrame, UIID::LaseWat_2, UIRect(80, 50, 50, 30),	"L2", btnTextSz);
	this->ApplyFormButtonStyle(this->btnLaseW_2);
	SetButtonStdCols(this->btnLaseW_2, false);
	this->btnLaseW_3 = new UIButton(this->inspSetFrame, UIID::LaseWat_3, UIRect(140, 50, 50, 30),	"L3", btnTextSz);
	this->ApplyFormButtonStyle(this->btnLaseW_3);
	SetButtonStdCols(this->btnLaseW_3, false);
	UIText* textLaseExp = new UIText(this->inspSetFrame, -1, "EXPOSURE", 20, UIRect(0.0f, 140.0f, 380, 20.0f));
	textLaseExp->uiCols.norm.SetColor_Black();
	this->btnExp_1	= new UIButton(this->inspSetFrame, UIID::Lase_Exposure_1, UIRect(20, 150, 50, 30), "E1", btnTextSz);
	this->ApplyFormButtonStyle(this->btnExp_1);
	SetButtonStdCols(this->btnExp_1, true);
	this->btnExp_2	= new UIButton(this->inspSetFrame, UIID::Lase_Exposure_2, UIRect(80, 150, 50, 30), "E2", btnTextSz);
	this->ApplyFormButtonStyle(this->btnExp_2);
	SetButtonStdCols(this->btnExp_2, false);
	this->btnExp_3	= new UIButton(this->inspSetFrame, UIID::Lase_Exposure_3, UIRect(20, 200, 50, 30), "E3", btnTextSz);
	this->ApplyFormButtonStyle(this->btnExp_3);
	SetButtonStdCols(this->btnExp_3, false);
	this->btnExp_4	= new UIButton(this->inspSetFrame, UIID::Lase_Exposure_4, UIRect(80, 200, 50, 30), "E4", btnTextSz);
	this->ApplyFormButtonStyle(this->btnExp_4);
	SetButtonStdCols(this->btnExp_4, false);
	this->btnThreshTy = new UIButton(inspSettingsPlate, UIID::Lase_ThresholdType, UIRect(20, 300, 100, 30), "THRESH:", btnTextSz);
	this->ApplyFormButtonStyle(this->btnThreshTy);
	SetButtonStdCols(this->btnThreshTy, false);
	// TODO: Initialize threshold based on starting value.
	this->btnThreshTog = new UIButton(inspSettingsPlate, UIID::Lase_ThresholdToggle, UIRect(120, 300, 100, 30), "ON", btnTextSz);
	this->ApplyFormButtonStyle(this->btnThreshTog);
	
	//
	const float titleHeight = 40.0f;
	this->inspAlignPlate	= new UIPlate( &this->uiSys, -1, defInspPlateDim, plateGray);
	this->inspAlignPlate->SetMode_Patch(this->patch_roundLeft, this->ninePatchCircle);
	this->inspAlignPlate->Show(true);
	UIText* alignTitle = new UIText(this->inspAlignPlate, -1, "Cam. Settings", 20, UIRect());
	alignTitle->debugName = "alignTitle";
	alignTitle->UseDyn()->AnchorsTop().SetOffsets(0.0f, 0.0f, 0.0f, titleHeight);
	{
		this->alignButtonGrid = new UIPlate(this->inspAlignPlate, -1, UIRect(), plateGray);
		this->alignButtonGrid->SetMode_Invisible();
		this->alignButtonGrid->UseDyn()->AnchorsAll().SetOffsets(0.0f, titleHeight, 0.0f, 0.0f);
		this->alignButtonGrid->Show(true);

		const float BtnHPad = 10.0f;
		const float BtnVPad = 10.0f;
		const float BtnH = 40.0f;
		const float BtnStride = BtnH + BtnVPad;
		int btnIt = BtnVPad;
		UIButton* btnSetExpo = new UIButton(this->alignButtonGrid, UIID::CamSet_Exposure, UIRect(), "EXPOSURE", btnTextSz);
		this->ApplyFormButtonStyle(btnSetExpo);
		btnSetExpo->UseDyn()->AnchorsTop().SetOffsets(BtnHPad, btnIt, -BtnHPad, btnIt + BtnH);
		SetButtonStdCols(btnSetExpo);
		btnIt += BtnStride;
		UIButton* btnSetDisp = new UIButton(this->alignButtonGrid, UIID::CamSet_Disparity, UIRect(), "DISPARITY", btnTextSz);
		this->ApplyFormButtonStyle(btnSetDisp);
		btnSetDisp->UseDyn()->AnchorsTop().SetOffsets(BtnHPad, btnIt, -BtnHPad, btnIt + BtnH);
		SetButtonStdCols(btnSetDisp);
		btnIt += BtnStride;
		UIButton* btnSetOpa = new UIButton(this->alignButtonGrid, UIID::CamSet_Opacity, UIRect(), "OPACITY", btnTextSz);
		this->ApplyFormButtonStyle(btnSetOpa);
		btnSetOpa->UseDyn()->AnchorsTop().SetOffsets(BtnHPad, btnIt, -BtnHPad, btnIt + BtnH);
		SetButtonStdCols(btnSetOpa);
		btnIt += BtnStride;
		UIButton* btnSetReg = new UIButton(this->alignButtonGrid, UIID::CamSet_Register, UIRect(), "REGISTER X/Y", btnTextSz);
		this->ApplyFormButtonStyle(btnSetReg);
		btnSetReg->UseDyn()->AnchorsTop().SetOffsets(BtnHPad, btnIt, -BtnHPad, btnIt + BtnH);
		SetButtonStdCols(btnSetReg);
		btnIt += BtnStride;
		UIButton* btnSetCal = new UIButton(this->alignButtonGrid, UIID::CamSet_Calibrate, UIRect(), "Calibrate Focus", btnTextSz);
		this->ApplyFormButtonStyle(btnSetCal);
		btnSetCal->UseDyn()->AnchorsTop().SetOffsets(BtnHPad, btnIt, -BtnHPad, btnIt + BtnH);
		SetButtonStdCols(btnSetCal);
		btnIt += BtnStride;
		UIButton* btnSetThr = new UIButton(this->alignButtonGrid, UIID::CamSet_Threshold, UIRect(), "Threshold Settings", btnTextSz);
		this->ApplyFormButtonStyle(btnSetThr);
		btnSetThr->UseDyn()->AnchorsTop().SetOffsets(BtnHPad, btnIt, -BtnHPad, btnIt + BtnH);
		SetButtonStdCols(btnSetThr);
		//btnIt += BtnStride;
	}
	{
		this->alignSubOpacity = new UIPlate(this->inspAlignPlate, -1, UIRect(), plateGray);
		this->alignSubOpacity->UseDyn()->AnchorsAll().SetOffsets(0.0f, titleHeight, 0.0f, 0.0f);
		this->alignSubOpacity->Show(false);

		UIPlate* titlePlate = new UIPlate(this->alignSubOpacity, -1, UIRect());
		titlePlate->UseDyn()->AnchorsTop().SetOffsets(10.0f, 0.0f, -10.0f, 40);
		titlePlate->uiCols.norm.SetColor_Black();
		UIText* titleText = new UIText(titlePlate, -1, "OPACITY", btnTextSz, UIRect());
		titleText->uiCols.norm.SetColor_White();
		titleText->UseDyn()->AnchorsAll().ZeroOffsets();

		this->sliderOpacity = new UIVBulkSlider(this->alignSubOpacity, UIID::CamSet_Opacity_Meter, 0.0f, 1.0f, 0.5f, UIRect());
		this->sliderOpacity->UseDyn()->AnchorsAll().SetOffsets(50.0f, 50.0f, -10.0f, -60.0f);

		UIButton* backBtn = new UIButton(this->alignSubOpacity, UIID::CamSet_Opacity_Back, UIRect(), "Back", btnTextSz);
		this->ApplyFormButtonStyle(backBtn);
		backBtn->textColor.SetColor_Black();
		backBtn->UseDyn()->AnchorsBot().SetOffsets(50.0f, -50.0f, -50.0f, -10.0f);
		SetButtonStdCols(backBtn);
	}
	{
		this->alignThreshold = new UIPlate(this->inspAlignPlate, -1, UIRect(), plateGray);
		this->alignThreshold->UseDyn()->AnchorsAll().SetOffsets(0.0f, titleHeight, 0.0f, 0.0f);
		this->alignThreshold->Show(false);

		UIPlate* titlePlate = new UIPlate(this->alignThreshold, -1, UIRect());
		titlePlate->UseDyn()->AnchorsTop().SetOffsets(10.0f, 0.0f, -10.0f, 40);
		titlePlate->uiCols.norm.SetColor_Black();
		UIText* titleText = new UIText(titlePlate, -1, "THRESHOLD", btnTextSz, UIRect());
		titleText->uiCols.norm.SetColor_White();
		titleText->UseDyn()->AnchorsAll().ZeroOffsets();

		this->sliderThresh = new UIVBulkSlider(this->alignThreshold, UIID::CamSet_Threshold_SlideThresh, 0.0f, 1.0f, 0.5f, UIRect());
		this->sliderThresh->UseDyn()->SetAnchors(0.0f, 0.0f, 0.5f, 1.0f).SetOffsets(10.0f, 50.0f, -10.0f, -60.0f);

		this->sliderDispup = new UIVBulkSlider(this->alignThreshold, UIID::CamSet_Threshold_DispUp, 0.0f, 1.0f, 0.5f, UIRect());
		this->sliderDispup->UseDyn()->SetAnchors(0.5f, 0.0f, 1.0f, 1.0f).SetOffsets(10.0f, 50.0f, -10.0f, -60.0f);

		UIButton* backBtn = new UIButton(this->alignThreshold, UIID::CamSet_Threshold_Back, UIRect(), "Back", btnTextSz);
		this->ApplyFormButtonStyle(backBtn);
		backBtn->textColor.SetColor_Black();
		backBtn->UseDyn()->AnchorsBot().SetOffsets(50.0f, -50.0f, -50.0f, -10.0f);
		SetButtonStdCols(backBtn);
	}
	//
	this->inspCamSetsPlate	= new UIPlate( &this->uiSys, -1, defInspPlateDim, plateGray);
	this->inspCamSetsPlate->Show(false);
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
		sz.x /2,		// Horizontally at the center
		sz.y / 2 + 500, // Near the bottom
		0.4f, 
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

	this->uiSys.DelegateMouseDown(button, UIVec2(pt.x, pt.y));
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
	// Step 1, hide all relevant inspector views.
	this->inspSettingsPlate->Hide();
	this->inspAlignPlate->Hide();

	UpdateGroupColorSet( 
		idx,
		{this->btnSettings, this->btnAlign, this->btnCamSets},
		colSetButtonTog,
		colSetButton);

	switch(idx)
	{
	case UIID::MBtnLaserSet:
		this->inspSettingsPlate->Show();
		break;

	case UIID::MBtnAlign:
		this->inspAlignPlate->Show();
		break;

	case UIID::MBtnSource:
		break;
	}
}

void StateHMDOp::ApplyFormButtonStyle(UIGraphic* uib)
{
	uib->SetMode_Patch(
		this->patch_smallCircle, 
		this->ninePatchSmallCircle);
}

void StateHMDOp::OnUISink_Clicked(UIBase* uib, int mouseBtn, const UIVec2& mousePos)
{
	// Only right click is supported
	if(mouseBtn != (int)MouseBtn::Right)
		return;

	switch(uib->Idx())
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
		break;

	case UIID::CamSet_Disparity:
		break;

	case UIID::CamSet_Opacity:
		this->alignButtonGrid->Hide();
		this->alignSubOpacity->Show();
		break;

	case UIID::CamSet_Register:
		break;

	case UIID::CamSet_Calibrate:
		break;

	case UIID::CamSet_Threshold:
		this->alignButtonGrid->Hide();
		this->alignThreshold->Show();
		break;

	case UIID::CamSet_Threshold_SlideThresh:
		break;

	case UIID::CamSet_Threshold_DispUp:
		break;

	case UIID::CamSet_Threshold_Back:
		this->alignThreshold->Hide();
		this->alignButtonGrid->Show();
		break;

	case UIID::CamSet_Opacity_Meter:
		break;

	case UIID::CamSet_Opacity_Back:
		this->alignSubOpacity->Hide();
		this->alignButtonGrid->Show();
		break;

	case UIID::CamSet_Calibrate_Slider:
		break;

	}
}