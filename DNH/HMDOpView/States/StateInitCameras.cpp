#include "StateInitCameras.h"
#include "StateIncludes.h"
#include "../CamStreamMgr.h"


StateInitCameras::StateInitCameras(HMDOpApp* app, GLWin* view, MainWin* core)
	: BaseState(BaseState::AppState::InitCams, app, view, core)
{
	this->nextState = false;
}

bool StateInitCameras::FlagTransitionNextState()
{
	if(this->lastObsrvState != CamStreamMgr::State::Polling)
		return false;

	this->nextState = true;
	return true;
}

void StateInitCameras::ClearVideoTexture()
{
	this->camFrame.Destroy();
}

void StateInitCameras::Draw(const wxSize& sz)
{
	this->mainFont.RenderFont(
		"Waiting for camera to connect",
		500, 
		100);

	cv::Ptr<cv::Mat> cur = 
		CamStreamMgr::GetInstance().GetCurrentFrame();

	this->lastObsrvState = 
		CamStreamMgr::GetInstance().GetState();

	bool showFeed = false;
	switch(this->lastObsrvState)
	{
		case CamStreamMgr::State::Unknown:
			this->mainFont.RenderFont("Booting",500, 200);
			break;
		case CamStreamMgr::State::Idling:
			this->mainFont.RenderFont("Feed could not connect.",500, 200);
			break;
		case CamStreamMgr::State::Connecting:
			this->mainFont.RenderFont("Connecting",500, 200);
			break;
		case CamStreamMgr::State::Polling:
			this->mainFont.RenderFont("Connected!\nPress any key to operate.",500, 200);
			showFeed = true;
			break;
		case CamStreamMgr::State::Shutdown:
			this->mainFont.RenderFont("Error, feed has ended.",500, 200);
			break;
	}

	if(showFeed == false)
		return;

	long long camMgrFrameID = CamStreamMgr::GetInstance().camFeedChanges;
	if(camMgrFrameID != this->lastFrameSeen)
	{
		this->lastFrameSeen = camMgrFrameID;
		cv::Ptr<cv::Mat> matptr = CamStreamMgr::GetInstance().GetCurrentFrame();
		this->camFrame.TransferFromCVMat(matptr);
	}
	if(this->camFrame.IsValid())
	{ 
		glColor3f(1.0f, 1.0f, 1.0f);
		glDisable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
		this->camFrame.GLBind();

		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(100.0f, 300.0f);
			//
			glTexCoord2f(1.0f, 0.0f);
			glVertex2f(500.0f, 300.0f);
			//
			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(500.0f, 600.0f);
			//
			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(100.0f, 600.0f);
		glEnd();
	}
}

void StateInitCameras::Update()
{
	if(this->nextState)
		this->GetCoreWindow()->ChangeState(BaseState::AppState::MainOp);
}

void StateInitCameras::EnteredActive()
{
	this->nextState = false;
}

void StateInitCameras::ExitedActive() 
{
	this->ClearVideoTexture();
}

void StateInitCameras::Initialize() 
{
	this->mainFont = FontMgr::GetInstance().GetFont(24);
	CamStreamMgr::GetInstance().BootConnectionToCamera();
}

void StateInitCameras::ClosingApp() 
{
	this->ClearVideoTexture();
}

void StateInitCameras::OnKeydown(wxKeyCode key)
{
	this->FlagTransitionNextState();
}

void StateInitCameras::OnMouseDown(int button, const wxPoint& pt)
{
	this->FlagTransitionNextState();
}

StateInitCameras::~StateInitCameras()
{
}