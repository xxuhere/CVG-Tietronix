#pragma once
#include "BaseState.h"
#include "../FontMgr.h"
#include "../Utils/cvgStopwatch.h"
#include "../TexObj.h"

/// <summary>
/// The first application state. The application is hard-coded
/// to go to the intro state. It doesn't do much except show a
/// title screen and flash some logos.
/// </summary>
class StateIntro : public BaseState
{
public:
	/// <summary>
	/// Had the user interacted with the app to
	/// allow transitioning to the next screen?
	/// </summary>
	bool interacted = false;

	FontWU mainFont;

	FontWU titleFont;
	FontWU nameFont;

	/// <summary>
	/// The backplate for the splash screens.
	/// </summary>
	TexObj::SPtr backplate;

	/// <summary>
	/// The logos for the organization listing badges.
	/// 
	/// For now, it's assumed they're all square (256x256)
	/// </summary>
	std::vector<TexObj::SPtr> logos;

	/// <summary>
	/// If true, the current session folder was found to already exist,
	/// and a warning should be shown on the screen.
	/// </summary>
	bool sessionAlreadyExists = false;


public:
	StateIntro(HMDOpApp* app, GLWin* view, MainWin* core);

	/// <summary>
	/// Reset the interaction flag.
	/// </summary>
	void ResetInteraction() { this->interacted = false; }

	/// <summary>
	/// When the user interacts with the application to go to
	/// the next screen, call this.
	/// </summary>
	void FlagInteracted() { this->interacted = true; }

	~StateIntro();

	//////////////////////////////////////////////////
	//
	//	BaseState Overrides
	//
	//////////////////////////////////////////////////

	void Draw(const wxSize& sz) override;
	void Update(double dt) override;

	void EnteredActive() override;
	void ExitedActive() override;

	void Initialize() override;
	void ClosingApp() override;

	void OnKeydown(wxKeyCode key) override;
	void OnMouseDown(int button, const wxPoint& pt) override;

};