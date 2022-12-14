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
	/// The location to draw the label font.
	/// </summary>
	UIVec2 labelRelPos;

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
	/// The amount to push the plate upwards/over. to emphasize
	/// what's selected. Note that this is needed in order to properly illustrate the effect
	/// with indentIn.
	/// </summary>
	float pushEmphasis;

	/// <summary>
	/// The amount to shorten the entry.
	/// - For horizontals this shortens the height.
	/// - For verticals this shortens the width.
	/// </summary>
	float trim;

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
		float trim,
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
/// The style of how to draw a Carousel
/// </summary>
class CarouselStyle
{
public:
	enum class Orientation
	{
		/// <summary>
		/// Layout carousel entries horizontally
		/// </summary>
		Horizontal,

		/// <summary>
		/// Layout carousel entries vertically
		/// </summary>
		Vertical
	};

private:
	Orientation orientation = Orientation::Horizontal;

public:

	/// <summary>
	/// If true, do an extra calculation to ensure the center
	/// of the seleted item is perfectly drawn in the specified
	/// center.
	/// 
	/// If false, the carousel will be in a "stable" mode, where the entire
	/// bounds of the carousel will be in (roughly) the same place, but
	/// the selected entry will move around.
	/// </summary>
	bool center = true;

	/// <summary>
	/// The speed of transitioning, where 1.0 is to perform
	/// 1 entry transition in one second, and 2 would be twice
	/// as fast (half a second).
	/// </summary>
	float transitionSpeed	= 10.0f;

	/// <summary>
	/// The dimension for compressed entries.
	/// 
	/// The width/height of an entry box that isn't the selected entry.
	/// ie., the width of a compressed entry box.
	/// 
	/// Whether it's the width or the height depends on the orientation
	/// of the style:
	/// - If horizontal, this would be the width.
	/// - If vertical, this would be the height.
	/// </summary>
	float boxComprDim		= 20.0f;

	/// <summary>
	/// The width of an expanded entry box.
	/// - For horizontal, this is the width of the seleted entry.
	/// - For vertical, this is the width of ALL entries.
	/// </summary>
	float boxExpandedWidth		= 100.0f;

	/// <summary>
	/// The height of an expanded entry box.
	/// - For horizontal orientation, this is the width of ALL entries.
	/// - For vertical, this is the height of the selected entry.
	/// </summary>
	float boxExpandedHeight		= 110.0f;

	/// <summary>
	/// How the carousel entries will be styled when they are the
	/// selected entry.
	/// </summary>
	CarouselMoment drawMoment_Active;

	/// <summary>
	/// How the carousel entries will be styled when they are the
	/// direct neighbor of the selected entry.
	/// </summary>
	CarouselMoment drawMoment_Nbr0;

	/// <summary>
	/// How the carousel entries will be styled when they are the
	/// next-next neighbor of the selected entry 
	/// (i.e., the selection's neighbor's neighbor).
	/// </summary>
	CarouselMoment drawMoment_Nbr1;

	/// <summary>
	/// How the carousel entries will be styled for entries beyond
	/// 2 neighbor distances from the selected entry.
	/// </summary>
	CarouselMoment drawMoment_Rest;

public:
	CarouselStyle();

	inline Orientation GetOrientation() const
	{ return this->orientation;}

	void SetOrientation(Orientation o);

	void SetDefaultMomentsHorizontal();
	void SetDefaultMomentsVertical();
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

protected:

	void ProcessUpdateAndDrawOrder(
		float x, 
		float y, 
		const CarouselStyle& style, 
		float scale,
		std::vector<Entry*>& drawOrder);

	void _UpdateAndRecacheScene_Horiz(const CarouselStyle& style);

	void _UpdateAndRecacheScene_Vert(const CarouselStyle& style);

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
	void Render(float x, float y, const CarouselStyle& style, float scale, const UIColor4& modColor);

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
	bool Append(const CarouselSystemData& csd);

	/// <summary>
	/// Add a range of carousel entries. Note that the carousel must not
	/// be loaded or else the request will be ignored.
	/// </summary>
	bool Append(const std::vector<CarouselData>& vec);

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

	/// <summary>
	/// Get the shorthand label of the currently selected item.
	/// </summary>
	/// <returns></returns>
	std::string GetCurrentLabel() const;

	/// <summary>
	/// Get the caption of the currently selected item.
	/// </summary>
	std::string GetCurrentCaption() const;

	/// <summary>
	/// Get the index of the currectly selected item.
	/// </summary>
	int GetCurrentIndex() const;

	/// <summary>
	/// Get the ID of the currently selected item.
	/// </summary>
	/// <returns></returns>
	std::string GetCurrentID() const;

	inline int GetIndexCount()
	{ return this->entries.size(); }

	/// <summary>
	/// Get the data for what the carousel currently has selected.
	/// </summary>
	/// <returns>
	/// The CarouselData of the currently selected item. Or, blank
	/// dummy-data if the carousel is empty.
	/// </returns>
	CarouselData GetCurrentData() const;

	bool AtStart() const;
	bool AtEnd() const;

	inline bool AnyMoreOnLeft() const
	{ return !this->AtStart(); }

	inline bool AnyMoreOnRight() const
	{ return !this->AtEnd(); }
};