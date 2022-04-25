#include "StateHMDOp.h"
#include "StateIncludes.h"
#include "../CamVideo/CamStreamMgr.h"
#include "../Utils/cvgShapes.h"

StateHMDOp::StateHMDOp(HMDOpApp* app, GLWin* view, MainWin* core)
	: BaseState(BaseState::AppState::MainOp, app, view, core)
{
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
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glBegin(GL_QUADS);
			viewRegion.GLVerts_Textured();
		glEnd();
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
	

	this->DrawMenuSystemAroundRect(cameraWindowRgn);

	// Draw debug timings
	const int camCt = 2;
	for(int i = 0; i < camCt; ++i)
	{
		std::stringstream sstrm;
		sstrm << "Cam: " << i << " - MS: " << camMgr.GetMSFrameTime(i);
		this->fontInsTitle.RenderFont(sstrm.str().c_str(), 0, sz.y - (20 * camCt) + (20 * i));
	}
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
	}
}

void StateHMDOp::EnteredActive()
{
	// While these texture loads were initially in Initialize(),
	// they've been moved to EnteredActive to deffer them, because
	// there was some texture loading issues on the Linux side -
	//
	// most likely because the OpenGL context doesn't get initialized
	// as fast as on Windows.
	if(!this->ico_MenuAlign.IsValid())
		this->ico_MenuAlign.LODEFromImage("Menu_Icon_Align.png");
	if(!this->ico_MenuSliders.IsValid())
		this->ico_MenuSliders.LODEFromImage("Menu_Icon_Sliders.png");
	if(!this->ico_MenuLaser.IsValid())
		this->ico_MenuLaser.LODEFromImage("Menu_Icon_Laser.png");
	if(!this->ico_MenuReturn.IsValid())
		this->ico_MenuReturn.LODEFromImage("Menu_Icon_Return.png");
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

void StateHMDOp::ClosingApp() 
{
}

StateHMDOp::~StateHMDOp()
{
}