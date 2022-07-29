#pragma once
#include "UIVec2.h"
#include <vector>
#include "../TexObj.h"

/// <summary>
/// A class that represents 
/// </summary>
struct NinePatcher
{
public:
	/// <summary>
	/// The top left of the inner stretchable area.
	/// </summary>
	UIVec2 inTopLeft;

	/// <summary>
	/// The bottom right of the inner stretchable area.
	/// </summary>
	UIVec2 inBotRight;

	/// <summary>
	/// The dimensions, in pixels, of how large to draw the
	/// top left region.
	/// </summary>
	UIVec2 pxTopLeft;

	/// <summary>
	/// The dimensions, in pixels, of how large to draw the
	/// bottom right region.
	/// </summary>
	UIVec2 pxBotRight;

public:
	NinePatcher();

	NinePatcher(
		const UIVec2& itl,
		const UIVec2& ibr,
		const UIVec2& pxtl,
		const UIVec2& pxbr);

	NinePatcher(
		const UIVec2& itl,
		const UIVec2& ibr,
		const UIVec2& imgSz);

	static NinePatcher MakeFromPixels(
		const UIVec2& imgSz, 
		const UIVec2& pxTL,
		const UIVec2& pxBR);

	static NinePatcher MakeFromPixels(
		const TexObj& tobj, 
		const UIVec2& pxTL,
		const UIVec2& pxBR);

	static NinePatcher MakeFromPixels(
		TexObj::SPtr pt, 
		const UIVec2& pxTL,
		const UIVec2& pxBR);

	static NinePatcher MakeFromPixelsUV(
		const TexObj& tobj, 
		const UIVec2& uvTL,
		const UIVec2& uvBR);

	static NinePatcher MakeFromPixelsUV(
		TexObj::SPtr pt, 
		const UIVec2& uvTL,
		const UIVec2& uvBR);

	/// <summary>
	/// Create a 9-patch based off an image where it's assumed the
	/// strechable middle is of 0 size, in the middle of the texture.
	/// </summary>
	/// <param name="tobj">The TexObj to create the 9-patch for.</param>
	static NinePatcher MakeFromPixelsMiddle(const TexObj& tobj);

	/// <summary>
	/// Create a 9-patch based off an image where it's assumed the
	/// strechable middle is of 0 size, in the middle of the texture.
	/// </summary>
	/// <param name="pt">The SPtr to the TexObj to create the 9-patch for.</param>
	static NinePatcher MakeFromPixelsMiddle(TexObj::SPtr pt);

	void Set(
		const UIVec2& itl,
		const UIVec2& ibr,
		const UIVec2& pxtl,
		const UIVec2& pxbr);

	void Set(
		const UIVec2& itl,
		const UIVec2& ibr,
		const UIVec2& imgSz);

	/// <summary>
	/// Calculate and dump the OpenGL GL_QUAD vertices for the 9-patch
	/// when made for a specific position and dimension.
	/// </summary>
	/// <param name="posOffs">The position (top left) of the 9-patch.</param>
	/// <param name="rgnDim">The dimensions to generate the 9-patch for.</param>
	/// <param name="outUVs">The destination to store calculated UVs.</param>
	/// <param name="outVerts">The destination to store calculated vertex positions.</param>
	void GeneratePatchGeometryQuads(
		const UIVec2& posOffs,
		const UIVec2& rgnDim,
		std::vector<UIVec2>& outUVs,
		std::vector<UIVec2>& outVerts);
};