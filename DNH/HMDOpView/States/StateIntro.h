#pragma once
#include "BaseState.h"
#include "../FontMgr.h"
#include "../Utils/cvgStopwatch.h"

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

	/// <summary>
	/// Timer for how long the state has been shown. This is used
	/// the drive procedural animations.
	/// </summary>
	cvgStopwatch loadScreenTimer;

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