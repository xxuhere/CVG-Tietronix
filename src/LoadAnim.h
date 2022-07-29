#pragma once
#include "TexObj.h"
#include "UISys/UIVec2.h"

/// <summary>
/// Static class encapsulating the resources and logic for the standard
/// animated loading icon.
/// </summary>
class LoadAnim
{
public:

	/// <summary>
	/// Return values for initializing assets.
	/// </summary>
	enum class LoadRet
	{
		Success,
		Error,
		AlreadyLoaded
	};

	/// <summary>
	/// An element in the return value of GetLoadTargs().
	/// </summary>
	struct LoadTarg
	{
		/// <summary>
		/// Pointer to the TexObject that should have the texture loaded.
		/// </summary>
		TexObj* dst;

		/// <summary>
		/// The location of the source image file to load into dst.
		/// </summary>
		const std::string srcFilepath;
	};

private:
	static TexObj texChevron;
	static TexObj texInner;
	static TexObj texClip;
	static TexObj texHalo;
	static TexObj texCog;

	/// <summary>
	/// This function is the authority of all the TextObj that need to
	/// be loaded from file for the LoadAnim to work properly.
	/// </summary>
	static std::vector<LoadTarg> GetLoadTargs();

	/// <summary>
	/// True if all resource images have been loaded.
	/// </summary>
	static bool allLoaded;

public:
	/// <summary>
	/// Ensures all TexObj objects needed by the LoadAnim system are
	/// loaded.
	/// </summary>
	static LoadRet EnsureInit();

	/// <summary>
	/// Release any images.
	/// </summary>
	static bool Uninit();

public:

	/// <summary>
	/// Draw the loading animation.
	/// 
	/// Requires all images to already be loaded.k
	/// </summary>
	/// <param name="pos">
	/// The screen position to draw the center of the animation.
	/// </param>
	/// <param name="scale">
	/// The scale of the animation. At scale 1, the animation will take
	/// up 200 pixels.
	/// </param>
	/// <param name="animOffset">
	/// The number of seconds that has passed since the start of the animation.
	/// The function invoker is in charge of keeping track of this.
	/// </param>
	static void DrawAt(const UIVec2& pos, float scale, float animOffset);
};