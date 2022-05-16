#include "StateInitCameras.h"
#include "StateIncludes.h"
#include "../CamVideo/CamStreamMgr.h"
#include "../LoadAnim.h"


std::string ConvertCVTypeToString(int ty)
{
	//https://stackoverflow.com/questions/10167534/how-to-find-out-what-type-of-a-mat-object-is-with-mattype-in-opencv

	int eleCt = ty / 8;
	int etype = ty % 8;

	static const char* szCVTable [8] = 
//	[0]		[1]		[2]		[3]		[4]		[5]		[6]
	{"8U",	"8S",	"16U",	"16S",	"32S",	"32F",	"64F"};

	std::string ret = "CV_";
	switch(etype)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
		ret += szCVTable[etype];
		break;
	default:
		ret += "UNKNOWN";
		break;
	}
	
	ret += " C(";
	ret += ('0' + eleCt);
	ret += ")";
	return ret;
}

StateInitCameras::StateInitCameras(HMDOpApp* app, GLWin* view, MainWin* core)
	: BaseState(BaseState::AppState::InitCams, app, view, core)
{
	this->nextState = false;
}

bool StateInitCameras::FlagTransitionNextState(bool force)
{
	// !TODO: Handle multiple cameras
	if(!force && !this->allCamsReady)
		return false;

	this->nextState = true;
	return true;
}

void StateInitCameras::ClearVideoTextures()
{
	this->camTextureRegistry.ClearTextures();
}

void StateInitCameras::Draw(const wxSize& sz)
{
	// If any camera is not ready, this gets
	// switched to false.
	

	const char* sztitle = 
		this->allCamsReady ? 
			"All cameras ready!" :
			"Waiting for camera to connect";

	float titleAdv = this->titleFont.GetAdvance(sztitle);
	this->titleFont.RenderFontCenter(sztitle, sz.x / 2, 150);

	if(this->allCamsReady)
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		this->mainFont.RenderFontCenter("Press any key to continue!",sz.x / 2, 250);
	}

	CamStreamMgr& camMgrInst = CamStreamMgr::GetInstance(); 

	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	int midScrX = sz.x / 2;
	int midScrY = sz.y / 2;

	// Timer for the load animation
	float loadAnimPhase = this->loadAnimTimer.Milliseconds(false) / 1000.0f;

	this->allCamsReady = true;

	for(int camIt = 0; camIt < 2; ++camIt)
	{
		cv::Ptr<cv::Mat> cur = 
			CamStreamMgr::GetInstance().GetCurrentFrame(camIt);

		ManagedCam::State camPollState = 
			CamStreamMgr::GetInstance().GetState(camIt);

		const int verticalOffsetBetweenCamPreviews = 350;
		int outpX = midScrX + 10;
		int outpY = 450 + camIt * verticalOffsetBetweenCamPreviews;

		static const float titleOffsetX = 30.0f;

		bool showFeed = false;
		switch(camPollState)
		{
			case ManagedCam::State::Unknown:
				this->mainFont.RenderFont("Booting",outpX + titleOffsetX, outpY);
				this->allCamsReady = false;
				break;
			case ManagedCam::State::Idling:
				this->mainFont.RenderFont("Feed could not connect.",outpX, outpY);
				this->allCamsReady = false;
				break;
			case ManagedCam::State::Connecting:
				this->mainFont.RenderFont("Connecting",outpX + titleOffsetX, outpY);
				this->allCamsReady = false;
				break;
			case ManagedCam::State::Polling:
				this->mainFont.RenderFont("Connected!",outpX + titleOffsetX, outpY);

				// Even when polling, there can be a state where a first frame hasn't
				// be polled yet.
				if(cur != nullptr)
				{
					const float tabIn = 50.0f;

					std::stringstream sstrmRes;
					sstrmRes << "Res: " << cur->cols << " x " << cur->rows;
					this->mainFont.RenderFont(sstrmRes.str().c_str(),outpX + tabIn, outpY + 20);

					std::stringstream sstrmChan;
					sstrmChan << "Channels: " << ConvertCVTypeToString(cur->type());
					this->mainFont.RenderFont(sstrmChan.str().c_str(),outpX + tabIn, outpY + 40);

					std::stringstream sstrmTiming;
					sstrmTiming << "Frame MS: " << camMgrInst.GetMSFrameTime(camIt) << " - frame " << camMgrInst.GetStreamFrameCt(camIt);
					this->mainFont.RenderFont(sstrmTiming.str().c_str(), outpX + tabIn, outpY + 60);
				}
				showFeed = true;
				break;
			case ManagedCam::State::Shutdown:
				this->allCamsReady = false;
				this->mainFont.RenderFont("Error, feed has ended.",outpX, outpY);
				break;
		}

		static const float prevViewWidth = 300.0f;
		static const float prevViewRadHeight = 100.0f; 
		// Only continue if there's a frame to update and draw.
		if(showFeed == false)
		{ 
			LoadAnim::DrawAt(UIVec2(outpX - prevViewWidth/2.0f, outpY), 1.0f, loadAnimPhase);
			continue;	
		}
		
		// Check if we need to update the image in OpenGL
		long long feedCtr = camMgrInst.GetCameraFeedChanges(camIt);
		cv::Ptr<cv::Mat> curImg = camMgrInst.GetCurrentFrame(camIt);
		this->camTextureRegistry.LoadTexture(camIt, curImg, feedCtr);
		cvgCamTextureRegistry::Entry texInfo = 
			this->camTextureRegistry.GetInfoCopy(camIt);

		// If everythings is good, render the camera feed.
		if(!texInfo.IsEmpty())
		{
			glColor3f(1.0f, 1.0f, 1.0f);

			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texInfo.glTexId);
			
			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f);
				glVertex2f(outpX - prevViewWidth,	outpY - prevViewRadHeight);
				//
				glTexCoord2f(1.0f, 0.0f);
				glVertex2f(outpX,					outpY - prevViewRadHeight);
				//
				glTexCoord2f(1.0f, 1.0f);
				glVertex2f(outpX,					outpY + prevViewRadHeight);
				//
				glTexCoord2f(0.0f, 1.0f);
				glVertex2f(outpX - prevViewWidth,	outpY + prevViewRadHeight);
			glEnd();
		}
	}
}

void StateInitCameras::Update(double dt)
{
	
	if(this->nextState)
		this->GetCoreWindow()->ChangeState(BaseState::AppState::MainOp);
}

void StateInitCameras::EnteredActive()
{
	// Leave as false and wait for OnDraw() to have a chance to properly
	// evaluate the state of this variable.
	this->allCamsReady = false;

	this->nextState = false;
	CamStreamMgr::GetInstance().BootConnectionToCamera(
		this->GetView()->cachedOptions.feedOpts);

	this->loadAnimTimer.Restart();
}

void StateInitCameras::ExitedActive() 
{
	this->ClearVideoTextures();
}

void StateInitCameras::Initialize() 
{
	this->mainFont = FontMgr::GetInstance().GetFont(24);
	this->titleFont = FontMgr::GetInstance().GetFont(40);
}

void StateInitCameras::ClosingApp() 
{
	this->ClearVideoTextures();
}

void StateInitCameras::OnKeydown(wxKeyCode key)
{

	bool forceTransition = (key == WXK_HOME);
	this->FlagTransitionNextState(forceTransition);
}

void StateInitCameras::OnMouseDown(int button, const wxPoint& pt)
{
	this->FlagTransitionNextState();
}

StateInitCameras::~StateInitCameras()
{
}