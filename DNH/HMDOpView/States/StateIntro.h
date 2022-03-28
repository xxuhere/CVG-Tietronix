#pragma once
#include "BaseState.h"
#include "../FontMgr.h"

class StateIntro : public BaseState
{
public:
	/// <summary>
	/// Had the user interacted with the app to
	/// allow transitioning to the next screen?
	/// </summary>
	bool interacted = false;

	FontWU mainFont;

public:
	StateIntro(HMDOpApp* app, GLWin* view, MainWin* core);

	void ResetInteraction() { this->interacted = false; }
	void FlagInteracted() { this->interacted = true; }

	~StateIntro();

	//////////////////////////////////////////////////
	//
	//	BaseState Overrides
	//
	//////////////////////////////////////////////////

	void Draw(const wxSize& sz) override;
	void Update() override;

	void EnteredActive() override;
	void ExitedActive() override;

	void Initialize() override;
	void ClosingApp() override;

	void OnKeydown(wxKeyCode key) override;
	void OnMouseDown(int button, const wxPoint& pt) override;

};