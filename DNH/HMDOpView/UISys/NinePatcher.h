#pragma once
#include "UIVec2.h"
#include <vector>
#include "../TexObj.h"

struct NinePatcher
{
public:
	UIVec2 inTopLeft;
	UIVec2 inBotRight;

	UIVec2 pxTopLeft;
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

	static NinePatcher MakeFromPixelsMiddle(const TexObj& tobj);
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


	void GeneratePatchGeometryQuads(
		UIVec2& posOffs,
		UIVec2& rgnDim,
		std::vector<UIVec2>& outUVs,
		std::vector<UIVec2>& outVerts);
};