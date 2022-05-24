#pragma once

#include "UIBase.h"
#include "../TexObj.h"
#include "../FontMgr.h"
#include "NinePatcher.h"

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
		/// Draw wireframe outline.
		/// </summary>
		Outline,

		/// <summary>
		/// Draw a colored quad.
		/// </summary>
		RawRect,

		/// <summary>
		/// Draw a textured quad
		/// </summary>
		TexRect,

		/// <summary>
		/// Draw a 9 patch quad
		/// </summary>
		Patch
	};

private:
	TexObj::SPtr plateImg;

	std::vector<UIVec2> uvs;
	std::vector<UIVec2> verts;

	PlateDraw plateDraw = PlateDraw::RawRect;

	NinePatcher ninePatch;

public:
	UIGraphic(UIBase* parent, int idx, const UIRect& r, const std::string& filepath);
	UIGraphic(UIBase* parent, int idx, const UIRect& r, TexObj::SPtr img);

	bool Render() override;
	void _RenderGraphic();

	void SetImage(TexObj::SPtr img);

	void SetMode_Invisible();
	//
	void SetMode_Outline();
	//
	void SetMode_RawRect();
	//
	void SetMode_TexRect();
	void SetMode_TexRect(TexObj::SPtr img);
	//
	void SetMode_Patch();
	void SetMode_Patch(TexObj::SPtr img, const UIVec2& pmin, const UIVec2& pmax);
	void SetMode_Patch(const NinePatcher& patch);
	void SetMode_Patch(TexObj::SPtr img, const NinePatcher& patch);

	void HandleDirty();

	static void FlushGLVerts(std::vector<UIVec2>& verts);
	static void FlushGLVerts(
		std::vector<UIVec2>& uvs,
		std::vector<UIVec2>& verts);
};