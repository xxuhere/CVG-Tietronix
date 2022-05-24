#include "NinePatcher.h"


NinePatcher::NinePatcher()
{
}

NinePatcher::NinePatcher(
	const UIVec2& itl,
	const UIVec2& ibr,
	const UIVec2& pxtl,
	const UIVec2& pxbr)
{
	this->Set(itl, ibr, pxtl, pxbr);
}

NinePatcher::NinePatcher(
	const UIVec2& itl,
	const UIVec2& ibr,
	const UIVec2& imgSz)
{
	this->Set(itl, ibr, imgSz);
}

NinePatcher NinePatcher::MakeFromPixels(
	const UIVec2& imgSz, 
	const UIVec2& pxTL,
	const UIVec2& pxBR)
{
	UIVec2 offsBR(imgSz.x - pxBR.x, imgSz.y - pxBR.y);

	UIVec2 uvTL(pxTL.x / imgSz.x, pxTL.y / imgSz.y);
	UIVec2 uvBR(
		offsBR.x / imgSz.x,
		offsBR.y / imgSz.y);

	return NinePatcher(uvTL, uvBR, pxTL, offsBR);
}

NinePatcher NinePatcher::MakeFromPixels(
	const TexObj& tobj, 
	const UIVec2& pxTL,
	const UIVec2& pxBR)
{
	if(!tobj.IsValid() || tobj.width == 0 || tobj.height == 0)
		return NinePatcher();

	return MakeFromPixels(
		UIVec2(tobj.width, tobj.height),
		pxTL,
		pxBR);
}

NinePatcher NinePatcher::MakeFromPixels(
	const TexObj::SPtr pt, 
	const UIVec2& pxTL,
	const UIVec2& pxBR)
{
	if(pt == nullptr)
		return NinePatcher();

	return MakeFromPixels(*pt.get(), pxTL, pxBR);
}

NinePatcher NinePatcher::MakeFromPixelsUV(
	const TexObj& tobj, 
	const UIVec2& uvTL,
	const UIVec2& uvBR)
{
	if(!tobj.IsValid() || tobj.width == 0 || tobj.height == 0)
		return NinePatcher();


	UIVec2 pxTL(uvTL.x * tobj.width, uvTL.y * tobj.height);
	UIVec2 pxBR(uvBR.x * tobj.width, uvBR.y * tobj.height);
	UIVec2 offsBR(tobj.width - pxBR.x, tobj.height - pxBR.y);

	return NinePatcher(uvTL, uvBR, pxTL, offsBR);
}

NinePatcher NinePatcher::MakeFromPixelsUV(
	const TexObj::SPtr pt, 
	const UIVec2& uvTL,
	const UIVec2& uvBR)
{
	if(pt == nullptr)
		return NinePatcher();

	return MakeFromPixelsUV(*pt.get(), uvTL, uvBR);
}

NinePatcher NinePatcher::MakeFromPixelsMiddle(const TexObj& tobj)
{
	return MakeFromPixelsUV(tobj, UIVec2(0.5f, 0.5f), UIVec2(0.5f, 0.5f));
}

NinePatcher NinePatcher::MakeFromPixelsMiddle(TexObj::SPtr pt)
{
	return MakeFromPixelsUV(pt, UIVec2(0.5f, 0.5f), UIVec2(0.5f, 0.5f));
}

void NinePatcher::Set(
	const UIVec2& itl,
	const UIVec2& ibr,
	const UIVec2& pxtl,
	const UIVec2& pxbr)
{
	this->inTopLeft = itl;
	this->inBotRight = ibr;
	this->pxTopLeft = pxtl;
	this->pxBotRight = pxbr;
}

void NinePatcher::Set(
	const UIVec2& itl,
	const UIVec2& ibr,
	const UIVec2& imgSz)
{
	this->inTopLeft = itl;
	this->inBotRight = ibr;
	
	this->pxTopLeft.Set(
		itl.x * imgSz.x, 
		itl.y * imgSz.y);
	//
	this->pxBotRight.Set(
		ibr.x * imgSz.x, 
		ibr.y * imgSz.y);
}


void NinePatcher::GeneratePatchGeometryQuads(
	UIVec2& posOffs,
	UIVec2& rgnDim,
	std::vector<UIVec2>& outUVs,
	std::vector<UIVec2>& outVerts)
{
	// Generate the UVs.
	const UIVec2& uitl = this->inTopLeft;
	const UIVec2& uibr = this->inBotRight;
	//
	const UIVec2 uv00(0.0f,		0.0f	);
	const UIVec2 uv01(uitl.x,	0.0f	);
	const UIVec2 uv02(uibr.x,	0.0f	);
	const UIVec2 uv03(1.0f,		0.0f	);
	const UIVec2 uv04(0.0f,		uitl.y	);
	const UIVec2 uv05(uitl.x,	uitl.y	);
	const UIVec2 uv06(uibr.x,	uitl.y	);
	const UIVec2 uv07(1.0f,		uitl.y	);
	const UIVec2 uv08(0.0f,		uibr.y	);
	const UIVec2 uv09(uitl.x,	uibr.y	);
	const UIVec2 uv10(uibr.x,	uibr.y	);
	const UIVec2 uv11(1.0f,		uibr.y	);
	const UIVec2 uv12(0.0f,		1.0f	);
	const UIVec2 uv13(uitl.x,	1.0f	);
	const UIVec2 uv14(uibr.x,	1.0f	);
	const UIVec2 uv15(1.0f,		1.0f	);
	
	const float pxX0 = posOffs.x;
	const float pxX1 = posOffs.x + this->pxTopLeft.x;
	const float pxX2 = posOffs.x + rgnDim.x - this->pxBotRight.x;
	const float pxX3 = posOffs.x + rgnDim.x;
	const float pxY0 = posOffs.y;
	const float pxY1 = posOffs.y + this->pxTopLeft.y;
	const float pxY2 = posOffs.y + rgnDim.y - this->pxBotRight.y;
	const float pxY3 = posOffs.y + rgnDim.y;
	//
	const UIVec2 px00(pxX0, pxY0);
	const UIVec2 px01(pxX1, pxY0);
	const UIVec2 px02(pxX2, pxY0);
	const UIVec2 px03(pxX3, pxY0);
	const UIVec2 px04(pxX0, pxY1);
	const UIVec2 px05(pxX1, pxY1);
	const UIVec2 px06(pxX2, pxY1);
	const UIVec2 px07(pxX3, pxY1);
	const UIVec2 px08(pxX0, pxY2);
	const UIVec2 px09(pxX1, pxY2);
	const UIVec2 px10(pxX2, pxY2);
	const UIVec2 px11(pxX3, pxY2);
	const UIVec2 px12(pxX0, pxY3);
	const UIVec2 px13(pxX1, pxY3);
	const UIVec2 px14(pxX2, pxY3);
	const UIVec2 px15(pxX3, pxY3);


	//	PATCH: TOP LEFT
	outUVs.push_back(uv00);
	outUVs.push_back(uv01);
	outUVs.push_back(uv05);
	outUVs.push_back(uv04);
	outVerts.push_back(px00);
	outVerts.push_back(px01);
	outVerts.push_back(px05);
	outVerts.push_back(px04);

	//	PATCH: TOP CENTER
	outUVs.push_back(uv01);
	outUVs.push_back(uv02);
	outUVs.push_back(uv06);
	outUVs.push_back(uv05);
	outVerts.push_back(px01);
	outVerts.push_back(px02);
	outVerts.push_back(px06);
	outVerts.push_back(px05);

	//	PATCH: TOP RIGHT
	outUVs.push_back(uv02);
	outUVs.push_back(uv03);
	outUVs.push_back(uv07);
	outUVs.push_back(uv06);
	outVerts.push_back(px02);
	outVerts.push_back(px03);
	outVerts.push_back(px07);
	outVerts.push_back(px06);

	//	PATCH: MIDDLE LEFT
	outUVs.push_back(uv04);
	outUVs.push_back(uv05);
	outUVs.push_back(uv09);
	outUVs.push_back(uv08);
	outVerts.push_back(px04);
	outVerts.push_back(px05);
	outVerts.push_back(px09);
	outVerts.push_back(px08);

	//	PATCH: MIDDLE CENTER
	outUVs.push_back(uv05);
	outUVs.push_back(uv06);
	outUVs.push_back(uv10);
	outUVs.push_back(uv09);
	outVerts.push_back(px05);
	outVerts.push_back(px06);
	outVerts.push_back(px10);
	outVerts.push_back(px09);

	//	PATCH: MIDDLE RIGHT
	outUVs.push_back(uv06);
	outUVs.push_back(uv07);
	outUVs.push_back(uv11);
	outUVs.push_back(uv10);
	outVerts.push_back(px06);
	outVerts.push_back(px07);
	outVerts.push_back(px11);
	outVerts.push_back(px10);

	//	PATCH: BOTTOM LEFT
	outUVs.push_back(uv08);
	outUVs.push_back(uv09);
	outUVs.push_back(uv13);
	outUVs.push_back(uv12);
	outVerts.push_back(px08);
	outVerts.push_back(px09);
	outVerts.push_back(px13);
	outVerts.push_back(px12);

	//	PATCH: BOTTOM CENTER
	outUVs.push_back(uv09);
	outUVs.push_back(uv10);
	outUVs.push_back(uv14);
	outUVs.push_back(uv13);
	outVerts.push_back(px09);
	outVerts.push_back(px10);
	outVerts.push_back(px14);
	outVerts.push_back(px13);

	//	PATCH: BOTTOM RIGHT
	outUVs.push_back(uv10);
	outUVs.push_back(uv11);
	outUVs.push_back(uv15);
	outUVs.push_back(uv14);
	outVerts.push_back(px10);
	outVerts.push_back(px11);
	outVerts.push_back(px15);
	outVerts.push_back(px14);
}