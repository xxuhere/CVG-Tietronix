#include "FontMgr.h"

#include <wx/glcanvas.h>
#include <iostream>
#include "Utils/cvgAssert.h"

FontWU::FontWU()
{
}

FontWU::FontWU(FontMgr* mgr, FTFont* font)
{
	this->valid = true;
	this->mgr	= mgr;
	this->font	= font;
}

void FontWU::RenderFont(const char * sz, float x, float y)
{
	if(!this->valid)
		return;

	glPushMatrix();
		glTranslatef(x, y, 0.0f);
		// FTGL uses a system where +Y is upwards, where we're using
		// raster/screen coordinates where +Y is down, so that vertical
		// frame of reference needs to be inverted.
		glScalef(1.0f, -1.0f, 0.0f);
		this->font->Render(sz, -1);
	glPopMatrix();
}

void FontWU::_RenderFontRaw(const char* sz)
{
	if(!this->valid)
		return;

	this->font->Render(sz, -1);
}

void FontWU::RenderFontCenter(const char * sz, float x, float y, bool vertCenter)
{
	float adv = this->GetAdvance(sz);

	if(vertCenter)
		y += this->TypeSize() / 2.0f;

	this->RenderFont(sz, x - adv * 0.5f, y);
}

void FontWU::RenderFont(const std::string& str, float x, float y)
{
	this->RenderFont(str.c_str(), x, y);
}

void FontWU::RenderFontCenter(const std::string& str, float x, float y, bool vertCenter)
{
	this->RenderFontCenter(str.c_str(), x, y, vertCenter);
}

void FontWU::_RenderFontRaw(const std::string& str)
{
	this->_RenderFontRaw(str.c_str());
}

float FontWU::GetAdvance(const char* sz)
{
	if(!this->valid)
		return 0.0f;

	return this->font->Advance(sz);
}

float FontWU::GetAdvance(const std::string& str)
{
	return this->GetAdvance(str.c_str());
}

float FontWU::LineHeight() const
{
	if(!this->valid)
		return 0.0f;

	return this->font->LineHeight();
}

float FontWU::TypeSize() const
{
	if(!this->valid)
		return 0.0f;

	return this->font->FaceSize();
}

FontMgr FontMgr::_inst;

FontMgr& FontMgr::GetInstance()
{
	return _inst;
}

bool FontMgr::ShutdownMgr()
{
	return _inst.Shutdown();
}

FontMgr::FontMgr()
{}

FontMgr::~FontMgr()
{}

FontWU FontMgr::GetFont(const std::string& path, int fontSz)
{
	if(this->_isShutdown == true)
	{
		// If we're shutdown, it should be empty, and we shouldn't
		// be filling any request that will require more allocated
		// memory.
		std::cerr << "Attempting to access fonts while shutting down." << std::endl;
		return FontWU();
	}

	if(fontSz < 1 || fontSz > 100)
	{
		std::cerr << "Attempting to access font of unsupported size, " << fontSz << "." << std::endl;
		return FontWU();
	}

	// Find the set of loaded fonts for the path
	//////////////////////////////////////////////////

	FontLoad* fl = nullptr;
	auto itFind = fontCache.find(path);
	if(itFind == fontCache.end())
	{
		// TODO: We should refactor this to bail out if it doesn't exist
		// and would fail if we actually tried to load it, instead of letting
		// the first section pass, only to bail on the second part if there's
		// an error.
		fl = new FontLoad();
		fl->fontPath = path;
		fontCache[path] = fl;
	}
	else
		fl = itFind->second;

	// Find the specific size of the font requested
	//////////////////////////////////////////////////

	auto itFindCol = fl->sizeCollection.find(fontSz);
	if(itFindCol != fl->sizeCollection.end())
		return FontWU(this, itFindCol->second);

	// A newly observed combination, make and cache
	//////////////////////////////////////////////////

	FTTextureFont* texFont = new FTGLTextureFont(path.c_str());
	if(texFont == nullptr)
		return FontWU();

	if(texFont->Error() || !texFont->FaceSize(fontSz))
	{
		std::cerr << "Error creating font at " << path << "of size " << fontSz << std::endl;
		delete texFont;
		return FontWU();
	}
	
	texFont->Depth((float)fontSz);
	texFont->CharMap(ft_encoding_unicode);
	
	fl->sizeCollection[fontSz] = texFont;
	return FontWU(this, texFont);
}

FontWU FontMgr::GetFont(int fontSz)
{
	// This should be in the same folder at the executable, and 
	// the CWD should be set to that directory.
	static std::string fontPath = "ArialRegular.ttf";
	return GetFont(fontPath, fontSz);
}

bool FontMgr::Shutdown()
{
	if(this->_isShutdown == true)
		return false;

	for(auto it : this->fontCache)
	{
		FontLoad* fl = it.second;

		for(auto itCols : fl->sizeCollection)
			delete itCols.second;

		delete fl;
	}

	this->fontCache.clear();

	this->_isShutdown = true;

	return true;
}

