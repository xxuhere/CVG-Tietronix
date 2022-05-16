#pragma once
#include "BaseState.h"
#include "StateIncludes.h"
#include "../Utils/cvgStopwatch.h"
#include "../FontMgr.h"
#include "../CamVideo/CamStreamMgr.h"
#include "../TexObj.h"
#include "../Utils/cvgCamTextureRegistry.h"
#include "../Utils/VideoPollType.h"
#include <map>

/// <summary>
/// The application state for when the application is loading and
/// ensuring the camera is sucessfully connected and streaming 
/// before proceeding the application to the main HMD Operator state.
/// </summary>
class StateInitCameras : public BaseState
{
public:

	struct CamTextureReg
	{
		int id;
		int lastSeem = -1;
		int GLuint = -1;
	};

	FontWU mainFont;
	FontWU titleFont;
	bool nextState = false;
	bool allCamsReady = false;

	cvgCamTextureRegistry camTextureRegistry;
	cvgStopwatch loadAnimTimer;

public:
	StateInitCameras(HMDOpApp* app, GLWin* view, MainWin* core);

	bool FlagTransitionNextState(bool force = false);

	void ClearVideoTextures();

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