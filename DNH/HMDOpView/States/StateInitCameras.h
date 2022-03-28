#pragma once
#include "BaseState.h"
#include "StateIncludes.h"
#include "../FontMgr.h"
#include "../CamStreamMgr.h"
#include "../TexObj.h"

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
	void Update() override;

	void EnteredActive() override;
	void ExitedActive() override;

	void Initialize() override;
	void ClosingApp() override;

	void OnKeydown(wxKeyCode key) override;
	void OnMouseDown(int button, const wxPoint& pt) override;
};