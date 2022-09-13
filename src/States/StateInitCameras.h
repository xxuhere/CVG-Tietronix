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

	/// <summary>
	/// The larger font at the top of the screen.
	/// </summary>
	FontWU titleFont;		

	/// <summary>
	/// The font used for drawing text information
	/// </summary>
	FontWU mainFont;

	/// <summary>
	/// If true, the menu should advance to the next state (StateHMDOp)
	/// </summary>
	bool nextState = false;

	/// <summary>
	/// If true, all cameras have been detected to be connected.
	/// </summary>
	bool allCamsReady = false;

	/// <summary>
	/// A cache of OpenGL textures holding the last known cameras for
	/// </summary>
	cvgCamTextureRegistry camTextureRegistry;

	/// <summary>
	/// Timer for how long the state has been shown, used to drive 
	/// cyclic procedural animations such as the loading animations.
	/// </summary>
	cvgStopwatch loadAnimTimer;

	/// <summary>
	/// Determines if the beep sound has been played yet.
	/// </summary>
	bool playBeepLatch = false;

public:
	StateInitCameras(HMDOpApp* app, GLWin* view, MainWin* core);

	/// <summary>
	/// Set a flag that the state should transition to the next state
	/// as soon as possible.
	/// 
	/// This is expected to be called when a key/mouse is pressed to
	/// progress the app state machine.
	/// </summary>
	/// <param name="force">
	/// If true, progress to the next state, even if cameras are not
	/// ready yet.
	/// </param>
	/// <returns>
	/// If false, the flag request was ignored because the conditions to
	/// transition to the next state are not met yet (i.e., the cameras
	/// are not streaming yet).
	/// </returns>
	bool FlagTransitionNextState(bool force = false);

	/// <summary>
	/// Release image data from this->camTextureRegistry. This will clear
	///  a little bit of OpenGL (texture) memory when it's no longer needed.
	/// </summary>
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