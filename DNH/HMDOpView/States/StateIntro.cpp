#include "StateIntro.h"
#include "StateIncludes.h"

#include <FTGL/ftgl.h>

StateIntro::StateIntro(HMDOpApp* app, GLWin* view, MainWin* core)
	: BaseState(BaseState::AppState::Intro, app, view, core)
{
	this->ResetInteraction();
}

void StateIntro::Draw(const wxSize& sz)
{
	glColor3f(0.5f, 1.0f, 1.0f);
	
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	
	glBegin(GL_TRIANGLES);
		glVertex2f(0.0f, 0.0f);
		glVertex2f(10.0f, 0.0f);
		glVertex2f(10.0f, 10.0f);
	glEnd();
	
	this->mainFont.RenderFont("Example intro screen.",		500, 100);
	this->mainFont.RenderFont("Press any key to continue",	500, 200);
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
}

void StateIntro::ExitedActive() 
{
}

void StateIntro::Initialize() 
{
	this->mainFont = FontMgr::GetInstance().GetFont(24);
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