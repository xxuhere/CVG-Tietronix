#pragma once

#include <string>
#include "../TexObj.h"
#include "../Utils/cvgStopwatch.h"
#include "../FontMgr.h"
#include <queue>
#include "../UISys/UIColor4.h"

enum class ButtonID
{
	Left = 0,
	Middle = 1,
	Right = 2,
	HoldMiddle = 3,
	Totalnum
};

enum class MessageType
{
	Down,
	Up,
	HoldUp
};

struct Message
{
public:
	int idx;
	MessageType msgTy;

public:
	Message(MessageType msgTy, int idx);
};

class IMousepadUIBehaviour
{
public:
	/// <summary>
	/// Get the path of the icon to load and use for the
	/// action's overlay of specific mouse pad buttons.
	/// </summary>
	/// <param name="bid">The button to provide an icon path for.</param>
	/// <returns>The path, or "" to return nothing.</returns>
	virtual std::string GetIconPath(ButtonID bid) = 0;

	/// <summary>
	/// Get the name of the action for a mouse pad button.
	/// </summary>
	/// <param name="bid">The button to get the name of.</param>
	/// <returns>The queried name.</returns>
	virtual std::string GetActionName(ButtonID bid) = 0;

	/// <summary>
	/// Specify if a button is active.
	/// </summary>
	/// <param name="bid">The button to query the usability status of.</param>
	/// <returns>If true, button is usable, else the button does nothing.</returns>
	virtual bool GetButtonUsable(ButtonID bid) = 0;

	virtual void HandleMessage(const Message& msg) = 0;
};

/// <summary>
/// Data to support caching button annotation data.
/// </summary>
struct BAnnoIcon
{
public:
	std::string path;
	TexObj::SPtr loaded;
};

class MousepadUI
{
public:
	class ButtonState
	{
	public:
		int buttonIdx;
		cvgStopwatch heldDownTimer;

		TexObj normal;
		TexObj pressed;
		TexObj hold;

		// How fast the color will animation from fully on/off. Value
		// is in seconds.
		const double clickDecayRate = 1.0f;

		// A value from [0.0, 1.0], showing the render power to use based
		// of when the button was last clicked.
		// A value of 0.0 means the button was clicked long ago.
		// A value of 1.0 means the button is pressed at the moment.
		float clickRecent = 0.0f;

		// True if the mouse button is currently down, else false.
		bool isDown = false;

	public:
		ButtonState(
			int buttonIdx,
			const std::string& normImgPath,
			const std::string& pressedImgPath,
			const std::string& holdimgPath);

		bool InitializeButtonGraphics(
			const std::string& normImgPath,
			const std::string& pressedImgPath,
			const std::string& holdimgPath);

		void Reset();
		void Decay(double dt);
		void FlagUp();
		void FlagDown();

		/// <summary>
		/// Create a ButtonTexSet with the correct state.
		/// 
		/// See DrawOffsetVertices for other parameters.
		/// </summary>
		/// Hold long has the button been pressed down? Ignored if idDown is false.</param>
		/// <param name="holdingThreshold">
		/// How long does a button need to be held down before it's considered holding?
		/// </param>
		void DrawOffsetVerticesForButtonSet(
			float x, 
			float y, 
			float px, 
			float py, 
			float scale,
			float holdingThreshold);

		UIColor4 GetMousepadColor();
	};

	/// <summary>
	/// The amount of time to hold a button to do a hold operation
	/// instead of a press operation.
	/// </summary>
	const float ButtonHoldTime = 1.0f;

private:
	bool initialized = false;

public:

	ButtonState btnLeft;
	ButtonState btnMiddle;
	ButtonState btnRight;

	TexObj ico_MousePadCrevice;

	/// <summary>
	/// Cached button annotation icons.
	/// </summary>
	std::map<std::string, BAnnoIcon> cachedBAnnoIcos;

	/// <summary>
	/// The font used for button annotations.
	/// </summary>
	FontWU fontInsBAnno;

public:
	MousepadUI();
	bool Initialize();

	void Update(double dt);

	/// <summary>
	/// Draw the mousepad graphic.
	/// </summary>
	/// <param name="x">The x position (screen pixel) of the center of the graphic.</param>
	/// <param name="y">The y position (screen pixel) of the center of the graphic.</param>
	/// <param name="scale">The scale of the graphic.</param>
	void Render(IMousepadUIBehaviour* uiProvider, float x, float y, float scale);
	void OnButtonUp(IMousepadUIBehaviour* uib, int button);
	void OnButtonDown(IMousepadUIBehaviour* uib, int button);

	ButtonState* GetButton(int buttonIdx);

	void Reset();

	TexObj::SPtr GetBAnnoIco(const std::string& path);

	void Shutdown();


};