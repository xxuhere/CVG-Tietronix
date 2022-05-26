#include "UIGraphic.h"
#include "../Utils/cvgAssert.h"
#include <wx/glcanvas.h>

UIGraphic::UIGraphic(UIBase* parent, int idx, const UIRect& r, const std::string& filepath)
	: UIBase(parent, idx, r)
{
	TexObj::SPtr img = TexObj::MakeSharedLODE(filepath);
	this->SetMode_TexRect(img);
}

UIGraphic::UIGraphic(UIBase* parent, int idx, const UIRect& r, TexObj::SPtr img)
	: UIBase(parent, idx, r)
{
	if(img == nullptr)
		this->SetMode_RawRect();
	else
		this->SetMode_TexRect(img);
}

void UIGraphic::_RebuildVerts()
{
	this->_RebuildVerts(this->rect);
}

void UIGraphic::_RebuildVerts(const UIRect& r)
{
	this->uvs.clear();
	this->verts.clear();

	switch(this->plateDraw)
	{
	case PlateDraw::Invisible:
		break;

	case PlateDraw::Outline:
		r.GLQuad(this->verts);
		break;

	case PlateDraw::RawRect:
		r.GLQuad(this->verts);
		break;

	case PlateDraw::TexRect:
		r.GLQuadTex(this->uvs, this->verts);
		break;

	case PlateDraw::Patch:
		this->ninePatch.GeneratePatchGeometryQuads(
			r.pos,
			r.dim,
			this->uvs,
			this->verts);
		break;
	}
	this->dirtyContents = false;
}

void UIGraphic::_DrawVerts()
{
	// Draw the vert with the correct render state
	if(this->plateDraw != PlateDraw::Invisible)
	{
		UIColor4& col = 
			this->uiCols.GetContexedColor(
				this->pressedCt, 
				this->isHovered);

		switch(this->plateDraw)
		{
		case PlateDraw::Invisible:
			// Do-nothing
			break;

		case PlateDraw::Outline:
		{
			cvgAssert(this->verts.size() >= 4, "Missing cached outline verts.");

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDisable(GL_TEXTURE_2D);
			glColor3fv(col.ar);

			glBegin(GL_QUADS);
			FlushGLVerts(this->verts);
			glEnd();

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		break;

		case PlateDraw::RawRect:
		{
			cvgAssert(this->verts.size() >= 4, "Missing cached RawRect verts.");

			glDisable(GL_TEXTURE_2D);
			glColor3fv(col.ar);

			glBegin(GL_QUADS);
			FlushGLVerts(this->verts);
			glEnd();
		}
		break;

		case PlateDraw::TexRect:
		{
			cvgAssert(this->verts.size()	>= 4, "Missing cached TextRect verts.");
			cvgAssert(this->uvs.size()		>= 4, "Missing cached TextRect UVs");

			glEnable(GL_TEXTURE_2D);
			glColor3fv(col.ar);

			if(this->plateImg)
				this->plateImg->GLBind();

			glBegin(GL_QUADS);
			FlushGLVerts(this->uvs, this->verts);
			glEnd();
		}
		break;

		case PlateDraw::Patch:
		{
			// 4 verts a quad, for a 3x3 grid of quads.
			cvgAssert(this->verts.size()	>= 4 * 9, "Missing cached Patch verts.");
			cvgAssert(this->uvs.size()		>= 4 * 9, "Missing cached Patch UVs.");

			glEnable(GL_TEXTURE_2D);
			glColor3fv(col.ar);

			if(this->plateImg)
				this->plateImg->GLBind();

			glBegin(GL_QUADS);
			FlushGLVerts(this->uvs, this->verts);
			glEnd();
		}
		break;
		}
	}
}

bool UIGraphic::Render()
{
	if(!this->selfVisible)
		return false;

	this->_RenderGraphic();

	return UIBase::Render();
}

void UIGraphic::_RenderGraphic()
{
	// Regenerate the vert data if dirty
	if(this->IsContentsDirty())
		this->_RebuildVerts();

	this->_DrawVerts();
}

void UIGraphic::SetImage(TexObj::SPtr img)
{
	this->plateImg = img;
}

void UIGraphic::SetMode_Invisible()
{
	this->plateDraw = PlateDraw::Invisible;
	this->FlagContentsDirty();
}

void UIGraphic::SetMode_Outline()
{
	this->plateDraw = PlateDraw::Outline;
	this->FlagContentsDirty();
}

void UIGraphic::SetMode_RawRect()
{
	this->plateDraw = PlateDraw::RawRect;
	this->FlagContentsDirty();
}

void UIGraphic::SetMode_TexRect()
{
	this->plateDraw = PlateDraw::TexRect;
	this->FlagContentsDirty();
}

void UIGraphic::SetMode_TexRect(TexObj::SPtr img)
{
	this->plateImg = img;
	this->plateDraw = PlateDraw::TexRect;
	this->FlagContentsDirty();
}

void UIGraphic::SetMode_Patch()
{
	this->plateDraw = PlateDraw::Patch;
	this->FlagContentsDirty();
}

void UIGraphic::SetMode_Patch(TexObj::SPtr img, const UIVec2& pmin, const UIVec2& pmax)
{
	this->plateDraw = PlateDraw::Patch;
	this->FlagContentsDirty();

	this->plateImg = img;
	this->ninePatch = 
		NinePatcher::MakeFromPixels(
			UIVec2(img->width, img->height), 
			pmin, 
			pmax);
}

void UIGraphic::SetMode_Patch(const NinePatcher& patch)
{
	this->plateDraw = PlateDraw::Patch;
	this->FlagContentsDirty();

	this->ninePatch = patch;
}

void UIGraphic::SetMode_Patch(TexObj::SPtr img, const NinePatcher& patch)
{
	this->plateDraw = PlateDraw::Patch;
	this->FlagContentsDirty();

	this->plateImg = img;
	this->ninePatch = patch;
}

void UIGraphic::FlushGLVerts(std::vector<UIVec2>& verts)
{
	// Placeholder implementation, consider replacing with
	// vertex array functions.
	const int ct = verts.size();
	for(int i = 0; i < ct; ++i)
		glVertex2fv((float*)&verts[i]);

}

void UIGraphic::FlushGLVerts(
	std::vector<UIVec2>& uvs,
	std::vector<UIVec2>& verts)
{
	// Placeholder implementation, consider replacing with
	// vertex array functions.

	cvgAssert(uvs.size() == verts.size(), "UV and verts array must share the same size.");
	const int ct = verts.size();
	for(int i = 0; i < ct; ++i)
	{
		glTexCoord2fv((float*)&uvs[i]);
		glVertex2fv((float*)&verts[i]);
	}
}