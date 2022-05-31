#pragma once

#include "BaseState.h"
#include "../FontMgr.h"
#include "../CamVideo/CamStreamMgr.h"
#include "../TexObj.h"
#include "../Utils/cvgRect.h"
#include "../Utils/cvgCamTextureRegistry.h"
#include "../UISys/UISys.h"
#include "../UISys/UIPlate.h"
#include "../UISys/UIText.h"
#include "../UISys/UIVBulkSlider.h"
#include "../UISys/UIHSlider.h"
#include "../Carousel/Carousel.h"

class UIButton;

/// <summary>
/// The application state for the main operator loop.
/// </summary>
class StateHMDOp : 
	public BaseState,
	public UISink
{
public:

	/// <summary>
	/// Animation state information for a button in the mousepad graphic.
	/// </summary>
	struct MouseDownState
	{
		// How fast the color will animation from fully on/off. Value
		// is in seconds.
		const double clickDecayRate = 1.0f;

	public:
		// The time since the button was last clicked.
		float sinceClick = 0.0f;

		// True if the mouse button is currently down, else false.
		bool isDown = false;

	public:
		void Reset();
		void Decay(double dt);
		void FlagUp();
		void FlagDown();
	};

	/// <summary>
	/// The UI IDs of all unique UI elements created in uiSys.
	/// </summary>
	enum UIID
	{
		// Main Menubar
		PullMenu,
		MBtnLaserTog,
		MBtnLaserSet,
		MBtnAlign,
		MBtnSource,
		MBtnBack,
		// Laser Menu
		LaseWat_1,
		LaseWat_2,
		LaseWat_3,
		Lase_Exposure_1,
		Lase_Exposure_2,
		Lase_Exposure_3,
		Lase_Exposure_4,
		Lase_Threshold_None,
		Lase_Threshold_Simple,
		Lase_Threshold_Mean2,
		Lase_Threshold_Yen,
		Lase_Threshold_YenSimple,
		// Camera Settings
		CamSet_Exposure,
		CamSet_Disparity,
		CamSet_Opacity,
		CamSet_Register,
		CamSet_Calibrate,
		CamSet_Threshold,
		CamSet_Threshold_SlideThresh,
		CamSet_Threshold_DispUp,
		CamSet_Threshold_Back,
		CamSet_Opacity_Meter,
		CamSet_Opacity_Back,
		CamSet_Calibrate_Slider,
	};
public:
	/// <summary>
	/// If true, the submenu is being shown, else, it is not.
	/// 
	/// Currently UNUSED but is a placeholder for the animation
	/// system to slide it in and out.
	/// </summary>
	bool inspectorShow = false;

	/// <summary>
	/// For the menu bar, what's the max width of the submenus.
	/// </summary>
	float maxInspectorWidth = 200.0f;

	/// <summary>
	/// The UI system.
	/// </summary>
	UISys uiSys;

	/// <summary>
	/// The max width of the main menubar.
	/// 
	/// i.e., The width of the main menubar when it's fully 
	/// deployed/slid-outwards (to the right).
	/// </summary>
	const float maxVertWidth = 100.0f;

	/// <summary>
	/// The min with of the main menubar.
	/// 
	/// i.e., The width of the main menubar when it's fully
	/// hidden/slid-inwards (to the left).
	/// </summary>
	const float minVertWidth = 30.0f;

	/// <summary>
	/// The sliding speed of the main menubar when transitioning
	/// between being deployed or hidden.
	/// 
	/// In the units of pixels per second.
	/// </summary>
	const float vertTransSpeed = 400.0f;


	/// <summary>
	/// The current width of the main menubar.
	/// 
	/// It will either be minVertWidth or maxVertWidth if hidden or
	/// deployed (respectively) - or it will be a value in between
	/// if it's in the middle of an animation transition.
	/// </summary>
	float curVertWidth = minVertWidth;

	/// <summary>
	/// If true, the vertical menu should be shown, else it 
	/// should be hidden.
	/// 
	/// This variable is used to drive the state of the main 
	/// menubar sliding animation.
	/// </summary>
	bool showVertMenu = false;

	/// <summary>
	/// A cache of the OpenGL textures for the last down camera
	/// frame images.
	/// </summary>
	cvgCamTextureRegistry camTextureRegistry;

	/// <summary>
	/// The font used to render titles.
	/// </summary>
	FontWU fontInsTitle;

	/// <summary>
	/// The lawn known pos/size of camera viewport - which can change if
	/// the AppOptions.json is reloaded with different positions.
	/// 
	/// This is used to detect changes, so things that wrap around that
	/// viewport can also be adjusted.
	/// </summary>
	// Note: We're reffering to the rectangle as the camera "viewport", this
	// should not be confused with screen/OpenGL viewport, which reffers to the
	// ENTIRE SCREEN the application is rendering onto.
	UIRect lastViewportRect;

	TexObj::SPtr texSliderSysPos;	// [+] decoration icon for the slider system
	TexObj::SPtr texSliderSysNeg;	// [-] decoration icon for the slider system

	//////////////////////////////////////////////////
	//
	//		UI Assets
	//
	//////////////////////////////////////////////////

	// These are for standard graphical elements. If there are
	// other states that need this, this needs to be moved out
	// of the class and somewhere more accessible to all states,
	// such as the GLWin.

	TexObj::SPtr patch_circle;			// Large circle for heavily rounded rectangles
	TexObj::SPtr patch_roundLeft;		// Rounded only on the left side, for the submenu
	TexObj::SPtr patch_roundRight;		// Rounded only on the right side, for the main menu
	//
	// The ninepatch settings will be the same for all patch_*
	// images above.
	NinePatcher ninePatchCircle;		

	TexObj::SPtr patch_smallCircle;
	NinePatcher ninePatchSmallCircle;

	//////////////////////////////////////////////////
	//
	//		MOUSE PAD GRAPHIC VARIABLES
	//
	//////////////////////////////////////////////////

	// Various icons for the mouse pad graphic
	TexObj ico_MousePadLeft;
	TexObj ico_MousePadRight;
	TexObj ico_MousePadCrevice;
	TexObj ico_MousePadBall;

	// Animation state information for the mouse
	// pad graphic buttons.
	MouseDownState mdsLeft;
	MouseDownState mdsRight;
	MouseDownState mdsMiddle;

	//////////////////////////////////////////////////
	//
	//		RIGHT MAIN MENU BAR
	//
	//////////////////////////////////////////////////
	UIPlate* vertMenuPlate		= nullptr;
	UIButton* btnLaser			= nullptr;
	UIButton* btnSettings		= nullptr;
	UIButton* btnAlign			= nullptr;
	UIButton* btnCamSets		= nullptr;
	UIButton* btnBack			= nullptr;

	//////////////////////////////////////////////////
	//
	//		LASER SETTINGS MENU
	//
	//////////////////////////////////////////////////
	UIPlate* inspSettingsPlate	= nullptr;
	UIPlate* inspSetFrame		= nullptr;
	UIButton* btnLaseW_1		= nullptr;
	UIButton* btnLaseW_2		= nullptr;
	UIButton* btnLaseW_3		= nullptr;
	UIButton* btnExp_1			= nullptr;
	UIButton* btnExp_2			= nullptr;
	UIButton* btnExp_3			= nullptr;
	UIButton* btnExp_4			= nullptr;

	UIButton* btnThreshSel_None			= nullptr;
	UIButton* btnThreshTog_Simple		= nullptr;
	UIButton* btnThreshTog_Mean2		= nullptr;
	UIButton* btnThreshTog_Yen			= nullptr;
	UIButton* btnThreshTog_YenSimple	= nullptr;

	UIPlate* plateSliderCalibrate	= nullptr;

	//////////////////////////////////////////////////
	//
	//		ALIGNMENT SETTINGS MENU
	//
	//////////////////////////////////////////////////
	UIPlate* inspAlignPlate			= nullptr;

	//////////////////////////////////////////////////
	//
	//		CAMERA SETTINGS MENU
	//
	//////////////////////////////////////////////////
	UIPlate* inspCamSetsPlate		= nullptr;
	UIPlate* camButtonGrid			= nullptr;

	UIButton* camBtnExposure		= nullptr;
	UIButton* camBtnDisparity		= nullptr;
	UIButton* camBtnOpacity			= nullptr;
	UIButton* camBtnCalibrate		= nullptr;
	UIButton* camBtnRegisterXY		= nullptr;
	UIButton* camBtnThresh			= nullptr;

	UIPlate* plateSliderOpacity		= nullptr;
	UIPlate* plateSliderThresh		= nullptr;
	UIHSlider* sliderThresh			= nullptr;
	UIPlate* plateSliderDispup		= nullptr;

	/// <summary>
	/// The last button selected for the Cam. Settings submenu, so that
	/// if it's switched and revisited, it can retain its same UI state.
	/// </summary>
	int lastCamButtonSel = -1;

	//////////////////////////////////////////////////

	/// <summary>
	/// A collection of all horizontal plates that are supposed to be docked
	/// to the bottom of the camera viewport. So if we need to process or hide
	/// them all, we can just blindly iterate through the elements of this vector.
	/// </summary>
	std::vector<UIBase*> horizontalFloatingUISys;

	bool showCarousel = false;
	CarouselStyle carouselStyle;
	Carousel carousel;

public:
	StateHMDOp(HMDOpApp* app, GLWin* view, MainWin* core);

	/// <summary>
	/// Draws the frame around the camera viewport.
	/// </summary>
	/// <param name="rectDrawAround"></param>
	void DrawMenuSystemAroundRect(const cvgRect& rectDrawAround);

protected:
	void _SyncImageProcessingSetUI();
	void _SyncThresholdSlider();

public:
	/// <summary>
	/// Draw the mousepad graphic.
	/// </summary>
	/// <param name="x">The x position (screen pixel) of the center of the graphic.</param>
	/// <param name="y">The y position (screen pixel) of the center of the graphic.</param>
	/// <param name="scale">The scale of the graphic.</param>
	/// <param name="ldown">UNUSED: Consider removal</param>
	/// <param name="rdown">UNUSED: Consider removal</param>
	/// <param name="mdown">UNUSED: Consider removal</param>
	void DrawMousePad(float x, float y, float scale, bool ldown, bool rdown, bool mdown);

	//		BaseState FUNCTIONS
	//
	//////////////////////////////////////////////////

	void Draw(const wxSize& sz) override;
	void Update(double dt) override;
	//
	void EnteredActive() override;
	void ExitedActive() override;
	//
	void OnKeydown(wxKeyCode key) override;
	void OnKeyup(wxKeyCode key) override;
	void OnMouseDown(int button, const wxPoint& pt) override;
	void OnMouseUp(int button, const wxPoint& pt) override;
	void OnMouseMove(const wxPoint& pt) override;
	//
	void Initialize() override;
	void ClosingApp() override;

	/// <summary>
	/// Manage when a button in the "Cam. Settings" group is pressed.
	/// 
	/// This will manage changing the drawing state to show it 
	/// toggled on, as well as 
	/// </summary>
	/// <param name="buttonID">
	/// The UIID of a button in the "Cam. Settings" group. Or -1 to
	/// deselect everything.
	/// </param>
	/// <param name="record">
	/// If true, record the buttonID as the new state.
	/// </param>
	void ManageCamButtonPressed(int buttonID, bool record);

	/// <summary>
	/// Query if any UI context is active.
	/// 
	/// NOTE: Right now it's an unimplemented placeholder.
	/// </summary>
	/// <returns>
	/// True if any UI context is active, else false.
	/// 
	/// Note: This should not be confused with if any UI is shown. 
	/// As UI elements can be shown but not active, such as an undeployed
	/// main menubar.
	/// </returns>
	bool AnyUIUp();

	/// <summary>
	/// Create a complex slider system that's designed to be arrayed
	/// under the camera viewport.
	/// </summary>
	/// <param name="parent">Parent UI widget.</param>
	/// <param name="id">
	/// The UI of the slider.
	/// 
	/// This should not be confused with the 
	/// </param>
	/// <param name="labelText">
	/// The string value of the label on the left side of the UI system.
	/// </param>
	/// <param name="minVal">
	/// The minimum value of the slider.
	/// </param>
	/// <param name="maxVal">
	/// The maximum value of the slider.
	/// </param>
	/// <param name="startingVal">
	/// The starting value of the slider.
	/// </param>
	/// <param name="r">
	/// The local pos/size of the UI system.
	/// </param>
	/// <param name="outSlider">
	/// Options output parameter to get access to the created slider
	/// </param>
	/// <returns>The root plate of the UI system.</returns>
	UIPlate* CreateSliderSystem(
		UIBase* parent, 
		int id,
		const std::string& labelText,
		float minVal,
		float maxVal,
		float startingVal,
		const UIRect& r,
		UIHSlider** outSlider = nullptr);

	~StateHMDOp();

	bool ShowCarousel( bool show = true);

	bool HideCarousel();

	bool ToggleCarousel();

	inline bool IsCarouselShown() const
	{ return this->showCarousel; }

protected:
	/// <summary>
	/// Toggle a main menubar option, highlighting its button, as well
	/// as showing its submenu.
	/// </summary>
	/// <param name="idx">
	/// The UIID of a main menubar button to select. Or -1 to
	/// select nothing.
	/// </param>
	void SetShownMenuBarUIPanel(int idx);

	/// <summary>
	/// Set a UI element to use the shared style across the entire UI.
	/// </summary>
	/// <param name="uib">The UI widget to set the style for.</param>
	void ApplyFormButtonStyle(UIGraphic* uib);

	void DoThresholdButton(int idxButton, ProcessingType type, bool skipSet = false);

public:

	//		UISink overrides
	//
	//////////////////////////////////////////////////
	void OnUISink_Clicked(UIBase* uib, int mouseBtn, const UIVec2& mousePos) override;
	void OnUISink_ChangeValue(UIBase* uib, float value, int vid) override;

};