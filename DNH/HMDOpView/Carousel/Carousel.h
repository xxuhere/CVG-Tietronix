#pragma once

#include <string>
#include <vector>
#include "../FontMgr.h"
#include "../TexObj.h"
#include "../Utils/cvgOptions.h"

// Note that while this is using the UISys utilities, it's not
// a part of the UISys.
#include "../UISys/UIRect.h"
#include "../UISys/UIColor4.h"


/// <summary>
/// The style of how to draw a Carousel
/// </summary>
class CarouselStyle
{
public:
	/// <summary>
	/// The speed of transitioning, where 1.0 is to perform
	/// 1 entry transition in one second, and 2 would be twice
	/// as fast (half a second).
	/// </summary>
	float transitionSpeed	= 10.0f;

	/// <summary>
	/// The width of an entry box that isn't the selected entry.
	/// ie., the width of a compressed entry box.
	/// </summary>
	float boxComprDimX		= 20.0f;

	/// <summary>
	/// The width of an entry box that is the selected entry.
	/// ie., the width of an expanded-width entry box.
	/// </summary>
	float boxExpandDimX		= 100.0f;

	/// <summary>
	/// The height of entry boxes.
	/// </summary>
	float boxHeightY		= 110.0f;
};

/// <summary>
/// A representation of how a carousel can look at a given 
/// moment.
/// 
/// Usually a "moment" will be categorized as a discrete entry
/// distance from the selected carousel entry position (where the
/// selected entry is of distance 0, and its direct neighbors are
/// 1, and their neighbors are 2, etc.).
/// </summary>
class CarouselMoment
{
public:
	/// <summary>
	/// The cached region of where the icon should be drawn.
	/// </summary>
	UIRect relIcon;

	/// <summary>
	/// The background/plate fill color.
	/// </summary>
	UIColor4 bgColor;

	/// <summary>
	/// The modulated icon color.
	/// </summary>
	UIColor4 iconColor;

	/// <summary>
	/// The color of the label.
	/// </summary>
	UIColor4 textColor;

	/// <summary>
	/// The color of the outline.
	/// </summary>
	UIColor4 outlineColor;

	/// <summary>
	/// The thickness of the outline
	/// </summary>
	float outlineThickness;

	/// <summary>
	/// The font size of the label.
	/// </summary>
	// currently unused, as that would require aquiring a 
	// new font for each unique font type used.
	float labelSize;

	/// <summary>
	/// The rotation of the label font.
	/// </summary>
	float labelRot;

	/// <summary>
	/// The amount to indent in - so items drawn on top will
	/// overlap and convey a sense of depth.
	/// </summary>
	float indentIn;

	/// <summary>
	/// The amount to push the plate upwards. Note that this
	/// is needed in order to properly illustrate the effect
	/// with indentIn.
	/// </summary>
	float pushVert;

	/// <summary>
	/// The location to draw the label font.
	/// </summary>
	UIVec2 labelRelPos;

public:
	CarouselMoment();

	CarouselMoment(
		const UIRect& relIcon, 
		const UIColor4& bgColor, 
		const UIColor4& iconColor,
		const UIColor4& textColor,
		const UIColor4& outlineColor,
		float outlineThickness,
		float labelSize,
		float labelRot,
		float indentIn,
		float pushVert,
		const UIVec2& labelRelPos);

	/// <summary>
	/// Linear interpolatte between two CarouselMoments.
	/// </summary>
	static CarouselMoment Lerp(
		const CarouselMoment& a, 
		const CarouselMoment& b, 
		float t);
};

/// <summary>
/// The carousel object.
/// </summary>
class Carousel
{
public:

	/// <summary>
	/// An entry in the carousel.
	/// </summary>
	class Entry : public CarouselData
	{
	public:
		/// <summary>
		/// The loaded icon for the carousel.
		/// </summary>
		TexObj::SPtr icon;

		/// <summary>
		/// Cache the index that the entry is in, in the 
		/// Carousel::entries vector.
		/// </summary>
		int cachedIndex;

		/// <summary>
		/// The plate region of the rect. This will be the entire
		/// filled in plate.
		/// </summary>
		UIRect plateRect;

		/// <summary>
		/// The client region of the rect. This will be the 
		/// plate rectangle without the indent applied.
		/// </summary>
		UIRect clientRect;

		/// <summary>
		/// The interpolated drawing properties to render the entry wityh.
		/// </summary>
		CarouselMoment drawDetails;

	public:
		Entry();

		Entry(
			const std::string& id, 
			const std::string& iconFilepath,
			const std::string& label,
			const std::string& caption);

		inline bool IsImageLoaded() const
		{ return this->icon != nullptr; }
	};

private:
	bool hasLoaded =false;

public:


	/// <summary>
	/// The entries of the carousel.
	/// </summary>
	std::vector<Entry> entries;

	/// <summary>
	/// The current selected entry of the carousel.
	/// </summary>
	int currentEntry = 0;

	/// <summary>
	/// The current rendered entry of the carousel. With the animation
	/// effects, it will lag behind currentEntry but move towards it. Note
	/// it's a float to represent animated tweens beween ids.
	/// </summary>
	float currentShown = 0.0f;

	/// <summary>
	/// Cache for checking if currentShown is dirty.
	/// </summary>
	float lastShown = -1.0f;

	/// <summary>
	/// The font to render labels with.
	/// </summary>
	//
	// It's arguable if this is the best location for this variable.
	FontWU labelFont;

public:

	/// <summary>
	/// Clear all entries in the Carousel.
	/// </summary>
	void Clear();

	/// <summary>
	/// Load the font and image assets for the Carousel. Note that once
	/// this is done, the carousel cannot have any more entries added 
	/// to it.
	/// </summary>
	bool LoadAssets(bool force = false);

	/// <summary>
	/// If the Carousel is in the middle a transition animation, jump
	/// the animation to be at the selected entry.
	/// </summary>
	void EndAnimation(const CarouselStyle& style, bool updateCache);

	/// <summary>
	/// Update the carousel to account for time that has ellapsed.
	/// </summary>
	/// <param name="dx">The seconds that have passed since the last Update.</param>
	void Update(const CarouselStyle& style, float dx);

	/// <summary>
	/// Update cached rendering value. This should be called any time
	/// renderable data has been changed.
	/// </summary>
	void UpdateAndRecacheScene(const CarouselStyle& style);

	/// <summary>
	/// Render the carousel to the screen.
	/// </summary>
	// NOTE: The scale is currently ignored.
	void Render(float x, float y, const CarouselStyle& style, float scale);

	/// <summary>
	/// Move the carousel selection to the right of the current selection.
	/// </summary>
	/// <param name="anim">
	/// If false, do not animate a transition and force the visuals
	/// to immediately be at the correct location.
	/// </param>
	/// <returns>True if successful; else, false.</returns>
	bool GotoNext(bool anim = true);

	/// <summary>
	/// <Move the carousel selection to the left of the current selection.
	/// </summary>
	/// <param name="anim">
	/// If false, do not animate a transition and force the visuals
	/// to immediately be at the correct location.
	/// </param>
	/// <returns>True if successful; else, false.</returns>
	bool GotoPrev(bool anim = true);

	/// <summary>
	/// Go to a specific vector index in the carousel.
	/// </summary>
	/// <param name="idx"></param>
	/// <param name="anim">
	/// If false, do not animate a transition and force the visuals
	/// to immediately be at the correct location.
	/// </param>
	/// <returns>True if successful; else, false.</returns>
	bool Goto(int idx, bool anim = true);

	/// <summary>
	/// Jump the carousel selection to the entry that matches a specified ID.
	/// </summary>
	/// <param name="id">The id to select.</param>
	/// <param name="anim">
	/// If false, do not animate a transition and force the visuals
	/// to immediately be at the correct location.
	/// </param>
	/// <returns>True if successful; else, false.</returns>
	bool Goto(const std::string& id, bool anim = true);

	/// <summary>
	/// Add a range of carousel entries. Note that the carousel must not
	/// be loaded or else the request will be ignored.
	/// </summary>
	bool Append(std::vector<CarouselData>& vec);

	/// <summary>
	/// Add an additiona carousel entry. Note that the carousel must not
	/// be loaded or else the request will be ignored.
	/// </summary>
	bool Append(const CarouselData& cd);

	/// <summary>
	/// Add an additiona carousel entry. Note that the carousel must not
	/// be loaded or else the request will be ignored.
	/// </summary>
	bool Append(
		const std::string& id, 
		const std::string& iconFilepath, 
		const std::string& label,
		const std::string& caption);

	std::string GetCurrentLabel() const;

	std::string GetCurrentCaption() const;

	int GetCurrentIndex() const;

	CarouselData GetCurrentData() const;
};