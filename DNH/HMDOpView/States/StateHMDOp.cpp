#include "StateHMDOp.h"
#include "StateIncludes.h"
#include "../CamVideo/CamStreamMgr.h"
#include "../Utils/cvgShapes.h"
#include "../UISys/UIPlate.h"
#include "../UISys/UIButton.h"
#include <cmath>

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
		uiSys(-1, UIRect(0, 0, 1920, 1080))
{
	this->vertMenuPlate = new UIPlate( -1, UIRect(960, 540, 50.0f, 100.0f));

	UIButton* btnLaser		= new UIButton(-1, UIRect(20.0f, -icoDim/2.0f, icoDim, icoDim), "Menu_Icon_Align.png"	);
	btnLaser->SetPivot(0.0f, 1.0f/5.0f);
	UIButton* btnSettings	= new UIButton(-1, UIRect(20.0f, -icoDim/2.0f, icoDim, icoDim), "Menu_Icon_Sliders.png");
	btnSettings->SetPivot(0.0f, 2.0f/5.0f);
	UIButton* btnAlign		= new UIButton(-1, UIRect(20.0f, -icoDim/2.0f, icoDim, icoDim), "Menu_Icon_Laser.png"	);
	btnAlign->SetPivot(0.0f, 3.0f/5.0f);
	UIButton* btnCamSets	= new UIButton(-1, UIRect(20.0f, -icoDim/2.0f, icoDim, icoDim), "Menu_Icon_Return.png"	);
	btnCamSets->SetPivot(0.0f, 4.0f/5.0f);
	this->vertMenuPlate->AddChild(btnLaser);
	this->vertMenuPlate->AddChild(btnSettings);
	this->vertMenuPlate->AddChild(btnAlign);
	this->vertMenuPlate->AddChild(btnCamSets);
	this->uiSys.AddChild(this->vertMenuPlate);
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

	cvgRect cameraWindowRgn = 
		cvgRect(
			cx - vrWidth * 0.5f, 
			cy - vrHeight * 0.5f, 
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
		viewRegion.x = cx - viewRegion.w * 0.5f;
		viewRegion.y = cy - viewRegion.h * 0.5f;

		glBindTexture(GL_TEXTURE_2D, texInfo.glTexId);

		if(camMgr.IsThresholded(camIt))
			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
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
		sz.x /2, 
		sz.y / 2 + 300, 
		0.5f, 
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
	this->vertMenuPlate->SetDim(curVertWidth, cameraWindowRgn.h - 50.0f);

	this->uiSys.Align();
	this->uiSys.Render(UIRect(0.0f, 0.0f, sz.x, sz.y));
}

void StateHMDOp::DrawMenuSystemAroundRect(const cvgRect& rectDrawAround)
{
	cvgRect cvgrOutl = rectDrawAround;
	cvgrOutl.Inflate(10.0f);
	glDisable(GL_TEXTURE_2D);
	glColor3f(0.5f, 0.5f, 0.5f);
	glBegin(GL_LINE_LOOP);
	cvgrOutl.GLVerts();
	glEnd();

	//////////////////////////////////////////////////
	//
	//		DRAW MAIN MENU
	//
	//////////////////////////////////////////////////

	std::vector<TexObj*> mmIcos = 
	{
		&this->ico_MenuLaser,
		&this->ico_MenuSliders, 
		&this->ico_MenuAlign, 
		&this->ico_MenuReturn
	};

	const int iconSz = 50;
	const int iconSep = 20;
	const int iconPad = 30;
	const int iconCt = mmIcos.size();
	const int mmTotWidth = iconSz + iconPad + iconPad;
	const int mmTotHeight = iconCt * iconSz + (iconCt - 1) * iconSep + 2 * iconPad;

	cvgRect cvgMMenu(
		cvgrOutl.EndX(), 
		cvgrOutl.y + cvgrOutl.h * 0.5f - mmTotHeight * 0.5f, 
		mmTotWidth, 
		mmTotHeight);

	glColor3f(0.25f, 0.25f, 0.25f);
	glBegin(GL_TRIANGLE_FAN);
		cvgShapes::DrawBoxRoundedRight(cvgMMenu, 20.0f);
	glEnd();

	// Draw the main menu icons
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
	glEnable(GL_BLEND);
	glColor3f(1.0f, 1.0f, 1.0f);

	cvgRect menuIconRect(
		cvgMMenu.x + iconPad, 
		cvgMMenu.y + iconPad, 
		iconSz, 
		iconSz);

	for(size_t i = 0; i < mmIcos.size(); ++i)
	{
		mmIcos[i]->GLBind();
		glBegin(GL_QUADS);
			menuIconRect.GLVerts_Textured();
		glEnd();

		menuIconRect.y += iconSz + iconSep;
	}

	//////////////////////////////////////////////////
	//
	//		DRAW INSPECTOR REGION
	//
	//////////////////////////////////////////////////

	glDisable(GL_TEXTURE_2D);
	glColor3f(0.25f, 0.25f, 0.25f);
	float insWidth = 250.0f;
	cvgRect cvgMIns(
		cvgrOutl.EndX() - insWidth, 
		cvgrOutl.y, 
		insWidth, 
		cvgrOutl.h);

	glBegin(GL_TRIANGLE_FAN);
		cvgShapes::DrawBoxRoundedLeft(cvgMIns, 20.0f);
	glEnd();

	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE);
	glColor3f(1.0f, 1.0f, 1.0f);
	this->fontInsTitle.RenderFontCenter(
		"Title", 
		cvgMIns.x + cvgMIns.w * 0.5f, 
		cvgMIns.y + 25);

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_LINE_LOOP);
		cvgShapes::DrawBoxRoundedLeft(cvgMIns, 20.0f);
	glEnd();
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

	if(showVertMenu)
		curVertWidth = (float)std::min<float>(maxVertWidth, curVertWidth + vertTransSpeed * dt);
	else
		curVertWidth = (float)std::max<float>(minVertWidth, curVertWidth - vertTransSpeed * dt);
}

void StateHMDOp::EnteredActive()
{
	// While these texture loads were initially in Initialize(),
	// they've been moved to EnteredActive to deffer them, because
	// there was some texture loading issues on the Linux side -
	//
	// most likely because the OpenGL context doesn't get initialized
	// as fast as on Windows.
	this->ico_MenuAlign.LODEIfEmpty("Menu_Icon_Align.png");
	this->ico_MenuSliders.LODEIfEmpty("Menu_Icon_Sliders.png");
	this->ico_MenuLaser.LODEIfEmpty("Menu_Icon_Laser.png");
	this->ico_MenuReturn.LODEIfEmpty("Menu_Icon_Return.png");

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
}

void StateHMDOp::OnMouseDown(int button, const wxPoint& pt)
{
	if(button == 0)
		this->mdsLeft.FlagDown();
	else if(button == 1)
		this->mdsMiddle.FlagDown();
	else if(button == 2)
		this->mdsRight.FlagDown();
}

void StateHMDOp::OnMouseUp(int button, const wxPoint& pt)
{
	if(button == 0)
		this->mdsLeft.FlagUp();
	else if(button == 1)
		this->mdsMiddle.FlagUp();
	else if(button == 2)
		this->mdsRight.FlagUp();
}

void StateHMDOp::ClosingApp() 
{
}

StateHMDOp::~StateHMDOp()
{
}