#pragma once

#include "UIVec2.h" // Bring in ::Lerp()

/// <summary>
/// 4 component color representation.
/// </summary>
class UIColor4
{
public:
	union
	{
		struct
		{
			float r;	// Red
			float g;	// Green
			float b;	// Blue
			float a;	// Alpha (opacity)
		};
		float ar[4];
	};

public:
	UIColor4();
	UIColor4(float r, float g, float b);
	UIColor4(float r, float g, float b, float a);

	void Set(float r, float g, float b);
	void Set(float r, float g, float b, float a);

	/// <summary>
	/// Call glColor3f on the color data.
	/// </summary>
	void GLColor3();

	/// <summary>
	/// Call glColor4f on the color data.
	/// </summary>
	void GLColor4();

	// Preset color setting utilities.
	inline void SetColor_Black()	{ this->Set(0.0f, 0.0f, 0.0f); }
	inline void SetColor_White()	{ this->Set(1.0f, 1.0f, 1.0f); }
	inline void SetColor_FRed()		{ this->Set(1.0f, 0.0f, 0.0f); }
	inline void SetColor_FGreen()	{ this->Set(0.0f, 1.0f, 0.0f); }
	inline void SetColor_FBlue()	{ this->Set(0.0f, 0.0f, 1.0f); }
	inline void SetColor_FCyan()	{ this->Set(0.0f, 1.0f, 1.0f); }
	inline void SetColor_FMagenta()	{ this->Set(1.0f, 1.0f, 0.0f); }

	bool operator== (const UIColor4& o) const;

	inline static UIColor4 Lerp(const UIColor4& a, const UIColor4& b, float t)
	{
		return UIColor4(
			::Lerp(a.r, b.r, t),
			::Lerp(a.g, b.g, t),
			::Lerp(a.b, b.b, t),
			::Lerp(a.a, b.a, t));
	}
};

/// <summary>
/// A combination of colors used to represent a widget
/// under different interaction states.
/// </summary>
struct ColorSetInteractable
{
public:
	/// <summary>
	/// The normal color when not in hover or pressed mode.
	/// </summary>
	UIColor4 norm;

	/// <summary>
	/// The color when the mouse is hovered over the widget.
	/// </summary>
	UIColor4 hover;

	/// <summary>
	/// The color when the mouse is pressed down on the widget.
	/// </summary>
	UIColor4 pressed;

public:
	ColorSetInteractable();

	ColorSetInteractable(
		const UIColor4& norm, 
		const UIColor4& hover, 
		const UIColor4& pressed);

	/// <summary>
	/// Set all colors in the object to the same color.
	/// </summary>
	/// <param name="all">The color to set everything to.</param>
	ColorSetInteractable(const UIColor4& all);

	void Set(
		const UIColor4& norm, 
		const UIColor4& hover, 
		const UIColor4& pressed);

	/// <summary>
	/// Set all colors to the same color value.
	/// </summary>
	/// <param name="all">The color to set everything to.</param>
	void SetAll(const UIColor4& all);

	/// <summary>
	/// Given UI context data, select the relevant color member that
	/// the widget should use.
	/// </summary>
	/// <param name="pressedCt">
	/// The number of mouse buttons currently pressed on the widget.
	/// </param>
	/// <param name="isHovering">
	/// True if the mouse is hovering over the widget; else, false.
	/// </param>
	/// <returns>
	/// The color that should be used to represent the widgets given
	/// the specified context.
	/// </returns>
	UIColor4& GetContexedColor(int pressedCt, bool isHovering);
};