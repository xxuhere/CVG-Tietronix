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
	Totalnum
};

/// <summary>
/// The different types of messages that can be handled by a IMousepadUIBehaviour.
/// </summary>
enum class MessageType
{
	/// <summary>
	/// The button was pressed down.
	/// </summary>
	Down,

	/// <summary>
	/// The button was released as a click.
	/// </summary>
	Up,

	/// <summary>
	/// The button was released, but only after being held past the
	/// threshold of time that makes it considered a hold instead of a click.
	/// </summary>
	HoldUp
};


/// <summary>
/// An input event that can be sent to IMousepadUIBehaviour to handle.
/// </summary>
struct Message
{
public:
	/// <summary>
	/// The button id.
	/// </summary>
	int idx;

	/// <summary>
	/// The input event type.
	/// </summary>
	MessageType msgTy;

public:
	Message(MessageType msgTy, int idx);
};

/// <summary>
/// A class that can be inherited, which allows it to provide annotations and
/// handle input messages from MousepadUI.
/// 
/// Basically it's a connection that tells MousepadUI what to say the inputs do,
/// and then to actually do those things when sed inputs happen.
/// </summary>
class IMousepadUIBehaviour
{

public:
	/// <summary>
	/// Get the path of the icon to load and use for the
	/// action's overlay of specific mouse pad buttons.
	/// </summary>
	/// <param name="bid">The button to provide an icon path for.</param>
	/// <param name="isHold">
	/// If true, the query is for hold events, else the query is for click events.
	/// </param>
	/// <returns>The path, or "" to return nothing.</returns>
	virtual std::string GetIconPath(ButtonID bid, bool isHold) = 0;

	/// <summary>
	/// Get the name of the action for a mouse pad button.
	/// </summary>
	/// <param name="bid">The button to get the name of.</param>
	/// <param name="isHold">
	/// If true, the query is for hold events, else the query is for click events.
	/// </param>
	/// <returns>The queried name.</returns>
	virtual std::string GetActionName(ButtonID bid, bool isHold) = 0;

	/// <summary>
	/// Specify if a button is active.
	/// </summary>
	/// <param name="bid">The button to query the usability status of.</param>
	/// <param name="isHold">
	/// If true, the query is for hold events, else the query is for click events.
	/// </param>
	/// <returns>If true, button is usable, else the button does nothing.</returns>
	virtual bool GetButtonUsable(ButtonID bid, bool isHold) = 0;

	/// <summary>
	/// Handler for input events.
	/// </summary>
	/// <param name="msg">The message that the IMousepadUIBehaviour is responding to.</param>
	virtual void HandleMessage(const Message& msg) = 0;
};

/// <summary>
/// Data to support caching button annotation data.
/// </summary>
struct BAnnoIcon
{
public:
	/// <summary>
	/// The file path to the image.
	/// </summary>
	std::string path;

	/// <summary>
	/// The loaded image.
	/// </summary>
	TexObj::SPtr loaded;
};

/// <summary>
/// The UI subsystem for showing the mouse buttons that currently being
/// pressed, as well as notifiying things (derived off of IMousepadUIBehaviour) 
/// when those input events occur.
/// </summary>
class MousepadUI
{
public:

	/// <summary>
	/// The representation of a button being managed in the MousepadUI.
	/// </summary>
	class ButtonState
	{
	public:
		/// <summary>
		/// A cached index of the button being represented by the object.
		/// </summary>
		int buttonIdx;

		/// <summary>
		/// A timer tracking how long the button has been pressed. It
		/// should only be used if isDown is true.
		/// </summary>
		cvgStopwatch heldDownTimer;

		/// <summary>
		/// The button image for the button under normal conditions.
		/// </summary>
		TexObj normal;

		/// <summary>
		/// The button image for when the button is being pressed.
		/// </summary>
		TexObj pressed;

		/// <summary>
		/// The button image for when the button has been held down long
		/// enough to be considered a hold.
		/// </summary>
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
		/// <summary>
		/// Constructor.
		/// 
		/// See InitializeButtonGraphics() parameters information.
		/// </summary>
		/// <param name="buttonIdx">The button index the object is managing.</param>
		ButtonState(
			int buttonIdx,
			const std::string& normImgPath,
			const std::string& pressedImgPath,
			const std::string& holdimgPath);

		/// <summary>
		/// Initialize the button graphics, if not already initialized.
		/// </summary>
		/// <param name="normImgPath"></param>
		/// <param name="pressedImgPath"></param>
		/// <param name="holdimgPath"></param>
		/// <returns></returns>
		bool InitializeButtonGraphics(
			const std::string& normImgPath,
			const std::string& pressedImgPath,
			const std::string& holdimgPath);

		/// <summary>
		/// Reset the state information of the button (to be unpressed).
		/// </summary>
		void Reset();

		/// <summary>
		/// When a button is pressed, it will be highlighted and fade back to
		/// normal over time. This fading is referred to as a Decay.
		/// 
		/// Instead of calling Decay() when the button color is actually animating,
		/// it will probably be simpler to simply always call Decay(), even if it's
		/// fully decayed, for simplicity sake.
		/// </summary>
		/// <param name="dt">The amount of time to decay the pressed button color.</param>
		void Decay(double dt);

		/// <summary>
		/// Notify the button that it should be representing an unpressed button state.
		/// </summary>
		void FlagUp();

		/// <summary>
		/// Notify the button that it should be representing a pressed button state.
		/// </summary>
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

		/// <summary>
		/// Get the color of the mousepad. This takes into account if the button
		/// is being held, if the button is being pressed, as well as the decay color.
		/// </summary>
		/// <returns>The color to use for buttons.</returns>
		UIColor4 GetMousepadColor();
	};

	/// <summary>
	/// The amount of time to hold a button to do a hold operation
	/// instead of a press operation.
	/// </summary>
	const float ButtonHoldTime = 1.0f;

private:
	/// <summary>
	/// Checks if Initialize() has already been called.
	/// </summary>
	bool initialized = false;

public:

	ButtonState btnLeft;
	ButtonState btnMiddle;
	ButtonState btnRight;


	// Draws black only in the top region of the middle mouse circle
	// to make sure stuff doesn't visually fill up that area.
	TexObj ico_MousePadCrevice;

	/// <summary>
	/// A black region that's slight larger than the button circle
	/// used by btnMiddle.
	/// </summary>
	TexObj ico_CircleBacking;

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

	/// <summary>
	/// Initialize the object and load the button graphics.
	/// 
	/// This can be called multiple times, but will only ever do anything the first time
	/// it's called.
	/// </summary>
	/// <returns>
	/// If false, the request was ignored because Initialize() was already previously called.
	/// </returns>
	bool Initialize();

	/// <summary>
	/// Update function to animation and manage state information over time.
	/// </summary>
	/// <param name="dt"></param>
	void Update(double dt);

	/// <summary>
	/// Draw the mousepad graphic.
	/// </summary>
	/// <param name="x">The x position (screen pixel) of the center of the graphic.</param>
	/// <param name="y">The y position (screen pixel) of the center of the graphic.</param>
	/// <param name="scale">The scale of the graphic.</param>
	void Render(IMousepadUIBehaviour* uiProvider, float x, float y, float scale);

	/// <summary>
	/// This should be called by a manager tell the system that a button has been released.
	/// </summary>
	/// <param name="uib">An optional IMousepadUIBehaviour to receive input messages.</param>
	/// <param name="button">The button that was pressed.</param>
	void OnButtonDown(IMousepadUIBehaviour* uib, int button);

	/// <summary>
	/// This should be called by a manager tell the system that a button has been pressed.
	/// </summary>
	/// <param name="uib">An optional IMousepadUIBehaviour to receive input messages.</param>
	/// <param name="button">The button that was released.</param>
	void OnButtonUp(IMousepadUIBehaviour* uib, int button);

	/// <summary>
	/// Get the button state for a button index.
	/// 
	/// Valid values are in the range of [0, 3). 
	/// i.e., [ButtonID::left, ButtonID::Totalnum).
	/// There are no saftey checks to make sure the parameters are valid.
	/// </summary>
	/// <param name="buttonIdx">The button state information to retrieve.</param>
	/// <returns>The specified button state.</returns>
	ButtonState* GetButton(int buttonIdx);

	/// <summary>
	/// Reset the button state for all buttons.
	/// </summary>
	void Reset();

	/// <summary>
	/// Get an image based off a image file path.
	/// 
	/// Note that paths will only be loaded the first time. Any repeated parameters will be
	/// retrieved from a cached dictionary.
	/// </summary>
	/// <param name="path">The path to load from.</param>
	/// <returns>The loaded image.</returns>
	TexObj::SPtr GetBAnnoIco(const std::string& path);

	/// <summary>
	/// This should be called when MousepadUI is no longer needed, but before the OpenGL context
	/// is destroyed.
	/// </summary>
	void Shutdown();


};