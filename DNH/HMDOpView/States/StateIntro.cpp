#include "StateIntro.h"
#include "StateIncludes.h"
#include <FTGL/ftgl.h>
#include "../UISys/UIRect.h"

StateIntro::StateIntro(HMDOpApp* app, GLWin* view, MainWin* core)
	: BaseState(BaseState::AppState::Intro, app, view, core)
{
	this->ResetInteraction();
}

void StateIntro::Draw(const wxSize& sz)
{
	UIVec2 uihalf(sz.x / 2, sz.y / 2);

	UIRect rectPlate(
		(sz.x - this->backplate->width) * 0.5f,
		(sz.y - this->backplate->height) * 0.5f,
		this->backplate->width, 
		this->backplate->height);
	// Center the plate in the screen
	
	glColor3f(1.0f, 1.0f, 1.0f);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	this->backplate->GLBind();
	rectPlate.GLQuadTex();

	const float logoDim = 100.0f;
	const float logoStride = 120.0f;
	//
	// The left side of the right-most logo
	// Arbitrary value to match with how plate is positioned.
	// Offset from the center - because rectPlate is offset from the
	// center, it will stay aligned, even if not on the ideal
	float xLogo = 535; 
	float yLogo = 120;
	for(TexObj::SPtr ico : this->logos)
	{
		ico->GLBind();
		UIRect r(uihalf.x + xLogo, uihalf.y + yLogo, logoDim, logoDim);
		r.GLQuadTex();

		xLogo -= logoStride;
	}
	
	this->mainFont.RenderFontCenter(
		"Press any key to continue",
		uihalf.x, 
		sz.y - 50.0f);

	glColor3f(0.0f, 0.0f, 0.0f);

	// Just like the logos, the text offsets should be based off
	// the center.
	this->titleFont.RenderFont(
		"CVG",
		uihalf.x - 410.0f, 
		uihalf.y - 70.0f);

	this->nameFont.RenderFontCenter(
		"Fleuroscopic Visibility Enhancement",
		uihalf.x, 
		uihalf.y);

}

void StateIntro::Update(double dt)
{
	if(this->interacted)
	{
		std::cout << "Transitioning to init cam window" << std::endl;
		this->GetCoreWindow()->ChangeState(BaseState::AppState::InitCams);
	}
}

void StateIntro::EnteredActive()
{
	this->ResetInteraction();
	this->backplate = TexObj::MakeSharedLODE("Assets/Splash/Splash_Backplate.png");

	// The order listing will define how the badges are displayed,
	// from right to left order.
	//
	// FOR NOW THIS IS AN UNAUTHORIZED EXAMPLE!
	// BEFORE USAGE- VERIFY PERMISSIONS TO USE EACH LOGO!
	// (wleu 06/01/2022)
	std::vector<const char*> orgBadgesPaths = 
	{
		"Assets/Splash/Logo_UTSW.png",
		"Assets/Splash/Logo_WUSTL.png",
		"Assets/Splash/Logo_Tie.png",
		"Assets/Splash/Logo_NIH.png"
	};
	for(const char* szPngBadge : orgBadgesPaths)
	{
		TexObj::SPtr t = TexObj::MakeSharedLODE(szPngBadge);
		if(!t)
			continue;

		this->logos.push_back(t);
	}

	// Drawing states that will last the entire state.
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void StateIntro::ExitedActive() 
{
	// It's assumed this state will only be visited at the very
	// beginning, so after we leave, there's no reason to keep
	// these images in graphics memory.
	this->backplate = nullptr;
	this->logos.clear();
}

void StateIntro::Initialize() 
{
	this->mainFont = FontMgr::GetInstance().GetFont(24);

	// We may need to figure out a different plan here with these large
	// fonts than to simple allocate HUGE font sets.
	this->titleFont = FontMgr::GetInstance().GetFont(100);
	this->nameFont = FontMgr::GetInstance().GetFont(50);
}

void StateIntro::ClosingApp() 
{
}

StateIntro::~StateIntro()
{
}

void StateIntro::OnKeydown(wxKeyCode key)
{
	this->FlagInteracted();
}

void StateIntro::OnMouseDown(int button, const wxPoint& pt)
{
	this->FlagInteracted();
}