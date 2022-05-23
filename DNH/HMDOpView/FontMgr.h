#pragma once

#include <FTGL/ftgl.h>
#include <map>
#include <string>
#include "UISys/UIVec2.h"

class FontMgr;

/// <summary>
/// Font wrapper util to reference a loaded font at a specific size.
/// 
/// To render the FTGL fonts in this application, fonts need to be rendered
/// a certain way, this handle also limits how the FTGL font is accessed
/// to ensure proper use. See FontWU::RenderFont() for more details.
/// </summary>
class FontWU
{
private:
	/// <summary>
	/// FontWU objects can also represent null/invalid objects. If false,
	/// the mgr and font are not valid, and code should not attempt to
	/// use to FontWU for font rendering.
	/// </summary>
	bool valid		= false;

	/// <summary>
	/// Cached to the FontMgr. 
	/// Currently !UNUSED, this can probably be removed.
	/// </summary>
	FontMgr* mgr	= nullptr;

	/// <summary>
	/// The back-end font.
	/// </summary>
	FTFont* font	= nullptr;

public:
	/// <summary>
	/// Default constructor, create an invalid FontWU.
	/// </summary>
	FontWU();

	/// <summary>
	/// Create a valid FontWU referencing a font at a specific font size.
	/// 
	/// These should only be created through FontMgr::GetFont().
	/// </summary>
	FontWU(FontMgr* mgr, FTFont* font);

	bool IsValid(){ return this->valid;}

	/// <summary>
	/// Render font correctly for this application.
	/// </summary>
	/// <param name="sz">The string to render text for.</param>
	/// <param name="x">
	/// The x position, in screen coordinates, to render the text.
	/// </param>
	/// <param name="y">
	/// The y position, in screen coordinates, to render the text.
	/// </param>
	void RenderFont(const char * sz, float x, float y);
	void RenderFontCenter(const char * sz, float x, float y, bool vertCenter = false);

	inline void RenderFontCenter(const char * sz, const UIVec2& pos, bool vertCenter = false)
	{ return this->RenderFontCenter(sz, pos.x, pos.y, vertCenter); }

	float GetAdvance(const char* sz);
	float LineHeight() const;
	float TypeSize() const;
};

/// <summary>
/// Font manager, to allow the application to get font rendering 
/// utilities in a standardized and easy way.
/// </summary>
class FontMgr
{
private:
	/// <summary>
	/// A collection of fonts, of various font sizes, of a similar
	/// font type (file).
	/// </summary>
	struct FontLoad
	{
		/// <summary>
		/// The font path that all elements of sizeCollection should be.
		/// Note that there is currently no protection to ensure fontPath
		/// is canonicalized.
		/// </summary>
		std::string fontPath;

		/// <summary>
		/// 
		/// </summary>
		std::map<int, FTTextureFont*> sizeCollection;
	};

private:
	/// <summary>
	/// Singleton instance.
	/// </summary>
	static FontMgr _inst;

public:
	/// <summary>
	/// Public accessor to the singleton instance.
	/// </summary>
	static FontMgr& GetInstance();

	/// <summary>
	/// This should be called at teh end of the application's
	/// lifetime to properly shutdown the singleton instance.
	/// </summary>
	/// <returns>Success value. This can be ignored.</returns>
	static bool ShutdownMgr();

public:

	/// <summary>
	/// The collection of font types with their various font sizes.
	/// </summary>
	std::map<std::string, FontLoad*> fontCache;

	/// <summary>
	/// Check if the FontMgr has been shutdown. If so, we should
	/// not expect code to make additional requests to it or expect
	/// any additional functionality from it.
	/// </summary>
	bool _isShutdown = false;

private:
	FontMgr();

public:
	~FontMgr();

	/// <summary>
	/// 
	/// </summary>
	/// <param name="path"></param>
	/// <param name="fontSz"></param>
	/// <returns></returns>
	FontWU GetFont(const std::string& path, int fontSz);

	/// <summary>
	/// Get a font, of the default (build in Arial font file) type,
	/// of a specified size.
	/// </summary>
	/// <param name="fontSz">The font size.</param>
	/// <returns>The default font type, of the request font size.</returns>
	FontWU GetFont(int fontSz);

	/// <summary>
	/// Shutdown the manager.
	/// </summary>
	/// <returns>Success value.</returns>
	/// <remarks>
	/// The success value can be ignored. It's not anticipated that any action
	/// or error handling will fix anything if it's shutting itself down.
	/// </remarks>
	bool Shutdown();
};

