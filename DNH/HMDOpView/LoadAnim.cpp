#include "LoadAnim.h"
#include "Utils/cvgRect.h"
#include "Utils/cvgShapes.h"
#include "UISys/UIColor4.h"
#include <iostream>

TexObj LoadAnim::texChevron;
TexObj LoadAnim::texInner;
TexObj LoadAnim::texClip;
TexObj LoadAnim::texHalo;
TexObj LoadAnim::texCog;

bool LoadAnim::allLoaded = false;

#define DIVF255(x) (x / 255.0f)

// NOTE: The codebase is currently at c++2017. As of writing this comment,
// std::lerp is too new to use because it's a C++2020 function.
inline float Lerp(float a, float b, float t)
{
	return a + (b-a) * t;
}

std::vector<LoadAnim::LoadTarg> LoadAnim::GetLoadTargs()
{
	std::vector<LoadAnim::LoadTarg> ret = 
	{
		{&texChevron,	"Load_Chevron.png"	},
		{&texInner,		"Load_Inner.png"	},
		{&texClip,		"Load_InnerClip.png"},
		{&texHalo,		"Load_Halo.png"		},
		{&texCog,		"Load_Cog.png"		}
	};

	return ret;
}

LoadAnim::LoadRet LoadAnim::EnsureInit()
{
	int initted = 0;
	int alreadyInited = 0;
	
	// Predefine everything we need to load to
	// mechanize
	std::vector<LoadTarg> toLoad = GetLoadTargs();
	for(LoadTarg& lt : toLoad)
	{
		if(lt.dst->IsValid())
		{
			++alreadyInited;
			continue;
		}

		std::string lodePath = lt.srcFilepath;
		TexObj* to = lt.dst;
		if(to->LODEIfEmpty(lodePath) == TexObj::ELoadRet::Success)
		{
			++initted;
		}
	}

	// The number of LoadAnim::tex* variables we 
	// expect to load
	if(alreadyInited == toLoad.size())
	{
		allLoaded = true;
		return LoadRet::AlreadyLoaded;
	}
	else if(initted + alreadyInited != (int)toLoad.size())
		return LoadRet::Error;

	return LoadRet::Success;
}

bool LoadAnim::Uninit()
{
	bool any = false;
	std::vector<LoadAnim::LoadTarg> toUninit = GetLoadTargs();
	for(LoadAnim::LoadTarg& lt : toUninit)
	{
		if(lt.dst->IsValid())
		{ 
			lt.dst->Destroy();
			any = true;
		}
	}

	allLoaded = false;
	return any;
}

void LoadAnim::DrawAt(const UIVec2& pos, float scale, float animOffset)
{
	const static float PI = 3.141598f;

	const float radHalo					= 100.0f;
	//
	// Some of the png values are build to scale to radHalo, so
	// their values come from the proportion of their asset files
	static const float radClip			= 31.3f; 
	static const float radInner			= 49.3f;
	static const float radCog			= 34.8f;

	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor3f(1.0f, 1.0f, 1.0f);

	glPushMatrix();
		glTranslatef(pos.x, pos.y, 0.0f);
		glScalef(scale, scale, 1.0f);

		texHalo.GLBind();
		cvgRect rectHalo(-radHalo, -radHalo, radHalo * 2.0f, radHalo * 2.0f);
		glBegin(GL_QUADS);
			cvgShapes::DrawBoxTex(rectHalo);
		glEnd();

		texInner.GLBind();
		cvgRect rectInner(-radInner, -radInner, radInner * 2.0f, radInner * 2.0f);
		glBegin(GL_QUADS);
			cvgShapes::DrawBoxTex(rectInner);
		glEnd();


		//////////////////////////////////////////////////
		//
		//		CHEVRON RENDERINGS
		//
		//////////////////////////////////////////////////
		static const float chevMinScale		= 0.15f;
		static const float chevMaxScale		= 0.25f;
		static const float chevRPS			= 0.25f; // Rotations per second
		static const float chevScaleInRot	= 2.0f; // This works best as an integer
		static const float chevMoveOutMin	= 15.0f; 
		static const float chevMoveOutMax	= 30.0f;
		// Lerp at the correct rate using a sine curve.
		float chevRotLam	= ((1.0f + sin(2.0f * PI * chevRPS * animOffset * chevScaleInRot))/2.0f);
		float chevScale		= Lerp(chevMinScale, chevMaxScale, chevRotLam);
		float chevShift		= Lerp(chevMoveOutMin, chevMoveOutMax, chevRotLam);
		//
		static const UIColor4 chevCols[3] = 
		{
			UIColor4(DIVF255( 36), DIVF255(188), DIVF255(251)),
			UIColor4(DIVF255( 11), DIVF255(137), DIVF255(238)),
			UIColor4(DIVF255(132), DIVF255(144), DIVF255(250)),
		};
		texChevron.GLBind();
		for(int chevIt = 0; chevIt < 3; ++chevIt)
		{
			static const float radCHx = 48.9f;
			static const float radCHy = 42.4f;

			float angle = animOffset * chevRPS * 360.0f + 120.0f * chevIt;
			glColor3fv(chevCols[chevIt].ar);
			glPushMatrix();
				glRotatef(angle, 0.0f, 0.0f, 1.0f);
				glTranslatef(chevShift, 0.0f, 0.0f);
				glScalef(chevScale, chevScale, 1.0f);
				cvgRect rectChev(-radCHx, -radCHy, radCHx * 2.0f, radCHy * 2.0f);
				glBegin(GL_QUADS);
					cvgShapes::DrawBoxTex(rectChev);
				glEnd();
			glPopMatrix();
		}

		glColor3f(1.0f, 1.0f, 1.0f);
		// Cog cycles per second
		static const float cogMinScale		= 0.10f;
		static const float cogMaxScale		= 0.25f;
		//
		float cogLam		= pow(chevRotLam, 2.0f); // Low values, but spikey near 1.0
		float cogScale		= Lerp(cogMaxScale, cogMinScale, cogLam);
		texCog.GLBind();
		cvgRect rectCog(-radCog, -radCog, radCog * 2.0f, radCog * 2.0f);
		glPushMatrix();
			glScalef(cogScale, cogScale, 1.0f);
			glBegin(GL_QUADS);
				cvgShapes::DrawBoxTex(rectCog);
			glEnd();
		glPopMatrix();
		
	glPopMatrix();
}