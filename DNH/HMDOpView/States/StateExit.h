#include "BaseState.h"
#include "../FontMgr.h"

/// <summary>
/// That state the user will wait in while the application
/// is exiting.
/// 
/// While the application is shutting down, there are some
/// things that may take a while. Instead of leaving the
/// user on another screen or in a frozen state, leave the
/// user waiting in a screen that provides context.
/// 
/// Note that entering this state will automatically
/// send a close request to the application.
/// </summary>
class StateExit : public BaseState
{
public:
	FontWU mainFont;

public:
	StateExit(HMDOpApp* app, GLWin* view, MainWin* core);
	~StateExit();

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