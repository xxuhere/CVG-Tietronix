#include "cvgCamTextureRegistry.h"
#include "../glext.h"

GLuint cvgCamTextureRegistry::GetID(int camIdx)
{
	auto it = this->entries.find(camIdx);
	if(it == this->entries.end())
		return (GLuint)-1;

	return it->second.glTexId;
}

bool cvgCamTextureRegistry::HasID(int camIdx)
{
	return this->entries.find(camIdx) != this->entries.end();
}

cvgCamTextureRegistry::Entry cvgCamTextureRegistry::GetInfoCopy(int camIdx)
{
	auto it = this->entries.find(camIdx);
	if(it == this->entries.end())
		return Entry();

	return it->second;
}

GLuint cvgCamTextureRegistry::LoadTexture(int camIdx, cv::Ptr<cv::Mat> img, long long lastSeen)
{
	// Assumes OpenGL context is active when function is invoked.

	if(img == nullptr || img->empty())
		return GetID(camIdx);

	GLuint texAlloc = (GLuint)-1;

	auto it = this->entries.find(camIdx);
	if(it == this->entries.end())
	{
		// If we have no record of it at all.
		Entry e;
		e.lastSeen = lastSeen;
		e.cachedCamIdx = camIdx;
		e.cachedWidth = img->cols;
		e.cachedHeight = img->rows;
		//
		glGenTextures(1, &texAlloc);
		e.glTexId = texAlloc;
		this->entries[camIdx] = e;

		glBindTexture(GL_TEXTURE_2D, texAlloc);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}
	else
	{
		// There's no detected change from when the last texture was loaded,
		// don't update the image.
		if(it->second.lastSeen == lastSeen)
			return it->second.glTexId;

		// If we have a record of it, but detect we need to update its image.
		it->second.lastSeen = lastSeen;
		it->second.cachedWidth = img->cols;
		it->second.cachedHeight = img->rows;
		texAlloc = it->second.glTexId;

		glBindTexture(GL_TEXTURE_2D, texAlloc);
	}
	// Regardless of how we got here, texAlloc should be assigned and it
	// should be binded to GL_TEXTURE_2D.
	//
	// NOTE: We still need to add support for other formats besides BGR,
	// specifically 8bit 1channel greyscale.
	glTexImage2D(
		GL_TEXTURE_2D, 
		0, 
		GL_RGB, 
		img->cols, 
		img->rows, 
		0, 
		GL_BGR, 
		GL_UNSIGNED_BYTE,
		img->ptr());

	return texAlloc;
}

bool cvgCamTextureRegistry::ClearTexture(int camIdx)
{
	// Assumes OpenGL context is active when function is invoked.
	auto it = this->entries.find(camIdx);
	if(it == this->entries.end())
		return false;

	bool ret = true;

	if(it->second.glTexId != (GLuint)-1)
		glDeleteTextures(1, &it->second.glTexId);

	this->entries.erase(it);
	return ret;
}

void cvgCamTextureRegistry::ClearTextures()
{
	// Assumes OpenGL context is active when function is invoked.
	for(auto& it : this->entries)
	{
		if(it.second.glTexId != (GLuint)-1)
			glDeleteTextures(1, &it.second.glTexId);
	}
	this->entries.clear();
}