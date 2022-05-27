#pragma once

#include "UIBase.h"
#include "../TexObj.h"
#include "../FontMgr.h"
#include "NinePatcher.h"

/// <summary>
/// Base class for UIBase subclasses that have different
/// drawing states depending on the state of interaction.
/// </summary>
class UIGraphic : public UIBase
{
public:
	enum class PlateDraw
	{
		/// <summary>
		/// Do not color
		/// </summary>
		Invisible,

		/// <summary>
		/// Draw wireframe outline along the UI widget's rect.
		/// </summary>
		Outline,

		/// <summary>
		/// Draw a colored quad that fills up the UI widget's rect.
		/// </summary>
		RawRect,

		/// <summary>
		/// Draw a textured quad that fills up the UI widget's rect.
		/// </summary>
		TexRect,

		/// <summary>
		/// Draw a 9 patch quad that fills up the UI widget's rect.
		/// </summary>
		Patch
	};

private:
	/// <summary>
	/// The draw mode.
	/// </summary>
	PlateDraw plateDraw = PlateDraw::RawRect;

	/// <summary>
	/// The cached texture coordinates to render texture 
	/// mapped quads. If used, this should be the same size 
	/// as verts.
	/// </summary>
	std::vector<UIVec2> uvs;

	/// <summary>
	/// The cached verts used to render quads.
	/// </summary>
	std::vector<UIVec2> verts;

	/// <summary>
	/// The plate image. Only used if plateDraw is 
	/// PlateDraw::TextRect or PlateDraw::Patch.
	/// </summary>
	TexObj::SPtr plateImg;

	/// <summary>
	/// The nine patch construction info for drawing in the 
	/// UI widget's rect. Only used if plateDraw is set to 
	/// PlateDraw::Patch.
	/// </summary>
	NinePatcher ninePatch;

public:
	UIGraphic(UIBase* parent, int idx, const UIRect& r, const std::string& filepath);
	UIGraphic(UIBase* parent, int idx, const UIRect& r, TexObj::SPtr img);

	/// <summary>
	/// Rebuild the verts and UV data with respect to the UI widget's
	/// rect and plateDraw setting.
	/// </summary>
	void _RebuildVerts();

	/// <summary>
	/// Rebuild the verts and UV data with respect to specified rect
	/// and the object's plateDraw setting.
	/// </summary>
	/// <param name="r">The rect to build drawing geometry for.</param>
	void _RebuildVerts(const UIRect& r);

	/// <summary>
	/// Draw the cached verts in the mode specified by plateDraw.
	/// </summary>
	void _DrawVerts();

	bool Render() override;

	/// <summary>
	/// Render the UI widget.
	/// Updates the drawing geometry if it's flagged as dirty.
	/// </summary>
	void _RenderGraphic();

	/// <summary>
	/// Set the UI widgets image. Only relevant if plateDraw is
	/// PlateDraw::TexRect or PlateDraw::Patch.
	/// </summary>
	/// <param name="img">The image to set.</param>
	void SetImage(TexObj::SPtr img);

	/// <summary>
	/// Set the UI widget to PlateDraw::Invisible.
	/// </summary>
	void SetMode_Invisible();
	
	/// <summary>
	/// Set the UI widget to PlateDraw::Outline.
	/// </summary>
	void SetMode_Outline();
	
	/// <summary>
	/// Set the UI widget to PlateDraw::RawRect.
	/// </summary>
	void SetMode_RawRect();
	
	/// <summary>
	/// Set the UI widget to PlateDraw::TexRect.
	/// </summary>
	void SetMode_TexRect();

	/// <summary>
	/// Set the UI widget to PlateDraw::TexRect.
	/// </summary>
	/// <param name="img">The image to draw in the rect.</param>
	void SetMode_TexRect(TexObj::SPtr img);
	
	/// <summary>
	/// Set the UI widget to PlateDraw::Patch.
	/// </summary>
	void SetMode_Patch();

	/// <summary>
	/// Set the UI widget to PlateDraw::Patch.
	/// This version takes an image, and pixel landmarks on the image
	/// to build the 9-patch construction info.
	/// </summary>
	/// <param name="img">The image to draw a 9-patch with.</param>
	/// <param name="pmin">
	/// The top left pixel location in the image for the 9-patch construction info.
	/// </param>
	/// <param name="pmax">
	/// The bottom right pixel location in the image for the 9-patch construction info.
	/// </param>
	void SetMode_Patch(TexObj::SPtr img, const UIVec2& pmin, const UIVec2& pmax);

	/// <summary>
	/// Set the UI widget to PlateDraw::Patch.
	/// </summary>
	/// <param name="patch">The 9-patch construction info to use.</param>
	void SetMode_Patch(const NinePatcher& patch);

	/// <summary>
	/// Set the UI widget to PlateDraw::Patch.
	/// </summary>
	/// <param name="img">The image to draw the 9-patch with.</param>
	/// <param name="patch">The 9-patch construction info.</param>
	void SetMode_Patch(TexObj::SPtr img, const NinePatcher& patch);

	/// <summary>
	/// Utiltity function dump vertices to OpenGL vert drawcalls.
	/// </summary>
	/// <param name="verts">The verts.</param>
	static void FlushGLVerts(std::vector<UIVec2>& verts);

	/// <summary>
	/// Utility function to dump texture mapped vertices to OpenGL vert drawcalls.
	/// </summary>
	/// <param name="uvs">The UVs</param>
	/// <param name="verts">The verts.</param>
	static void FlushGLVerts(
		std::vector<UIVec2>& uvs,
		std::vector<UIVec2>& verts);
};