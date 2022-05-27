#pragma once
#include "UIBase.h"
#include <string>
#include "UIGraphic.h"
#include <initializer_list>
#include <vector>

/// <summary>
/// A UI implementation of a clickable button.
/// </summary>
class UIButton : public UIGraphic
{
public:
	
	// The button has an optiona feature to show text in the
	// center of the button. If used, all 3 of these should
	//be set.
	std::string text;			// The text to display.
	FontWU font;				// The font used to render the text.
	UIColor4 textColor;			// The color of the text.

private:
	void _InitializeColors();

public:
	/// <summary>
	/// Lambda function that if set, will be called every time the
	/// button is clicked.
	/// </summary>
	std::function<void(int)> onClick;

public:
	/// <summary>
	/// Constructor for a button image, whose image is loaded from 
	/// file with the LodePNG library.
	/// </summary>
	UIButton(UIBase* parent, int idx, const UIRect& r, const std::string& filePath);

	/// <summary>
	/// Constructor for a button image.
	/// </summary>
	UIButton(UIBase* parent, int idx, const UIRect& r, TexObj::SPtr img);

	/// <summary>
	/// Constructor for a rectangular colored button with text inside.
	/// </summary>
	UIButton(
		UIBase* parent,
		int idx, 
		const UIRect& r, 
		const std::string& text, 
		int size,		// Font size
		const std::string& fontType = "");

	void ResetColorsToDefault();

	void HandleClick(int button) override;
	bool Render() override;
};

/// <summary>
/// Given a list of buttons and a selection UI id, color all 
/// buttons that are not the id as one color scheme, and anything 
/// matching the selected.
/// </summary>
/// <param name="selIdx">The UI id to give the selection color.</param>
/// <param name="elements">The elements to scan through.</param>
/// <param name="colsHit">The selection colors.</param>
/// <param name="colsOther">The colors for unselected items.</param>
/// <returns></returns>
int UpdateGroupColorSet( 
	int selIdx,
	std::initializer_list<UIButton*> elements,
	const ColorSetInteractable& colsHit,
	const ColorSetInteractable& colsOther);