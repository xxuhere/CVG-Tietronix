#pragma once
#include "BaseState.h"
#include "StateIncludes.h"
#include "../FontMgr.h"
#include "../CamStreamMgr.h"
#include "../TexObj.h"

/// <summary>
/// The application state for when the application is loading and
/// ensuring the camera is sucessfully connected and streaming 
/// before proceeding the application to the main HMD Operator state.
/// </summary>
class StateInitCameras : public BaseState
{
public:
	FontWU mainFont;
	CamStreamMgr::State lastObsrvState = CamStreamMgr::State::Unknown;
	bool nextState = false;

	TexObj camFrame;
	long long lastFrameSeen = -1;

public:
	StateInitCameras(HMDOpApp* app, GLWin* view, MainWin* core);

	bool FlagTransitionNextState();

	void ClearVideoTexture();

	~StateInitCameras();

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