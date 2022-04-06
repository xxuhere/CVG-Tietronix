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
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	long long camMgrFrameID = CamStreamMgr::GetInstance().camFeedChanges;
	if(camMgrFrameID != this->lastFrameSeen)
	{
		this->lastFrameSeen = camMgrFrameID;
		cv::Ptr<cv::Mat> matptr = CamStreamMgr::GetInstance().GetCurrentFrame();
		this->camFrame.TransferFromCVMat(matptr);
	}
	if(!this->camFrame.IsValid())
		return;

	float cx = sz.x / 2;
	float cy = sz.y / 2;
	cvgRect viewRegion = cvgRect::MakeWidthAspect(500.0f, this->camFrame.VAspect());
	viewRegion.x = cx - viewRegion.w * 0.5f;
	viewRegion.y = cy - viewRegion.h * 0.5f;
	
	glColor3f(1.0f, 1.0f, 1.0f);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	this->camFrame.GLBind();

	glBegin(GL_QUADS);
		viewRegion.GLVerts_Textured();
	glEnd();

	this->DrawMenuSystemAroundRect(viewRegion);
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
}

void StateHMDOp::Initialize() 
{
	this->fontInsTitle = FontMgr::GetInstance().GetFont(24);
}

void StateHMDOp::OnKeydown(wxKeyCode key)
{
	if(key == 'S')
		this->GetCoreWindow()->RequestSnap();
}

void StateHMDOp::ClosingApp() 
{
}

StateHMDOp::~StateHMDOp()
{
}