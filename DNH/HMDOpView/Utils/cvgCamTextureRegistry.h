#pragma once
#include <map>

// Not appropriate to load in wxWidgets OpenGL GUI items, but currently
// the most painless way to bring in OpenGL type defines without worrying
// about platform specific rules.
// (wleu 04/13/2022)
#include <wx/glcanvas.h>
#include <opencv2/core.hpp>

/// <summary>
/// A utility manager to hold loaded camera OpenGL textures.
/// </summary>
class cvgCamTextureRegistry
{
public:
	struct Entry
	{
	public:
		/// <summary>
		/// The cached entry index (camera ID) the entry is for.
		/// </summary>
		int cachedCamIdx = -1;

		/// <summary>
		/// The last counter the 
		/// </summary>
		long long lastSeen = -1;

		/// <summary>
		/// OpenGL textureID.
		/// </summary>
		GLuint glTexId = (GLuint)-1;

		int cachedWidth = -1;
		int cachedHeight = -1;

	public:
		inline bool IsEmpty()
		{ return this->glTexId == (GLuint)-1; }
	};

private:
	std::map<int, Entry> entries;

public:
	/// <summary>
	/// Note that the entry passed back is a COPY! Do not expect
	/// to edit it directly. This function is not to provide editable
	/// access, but to perform the equivalent of multiple properties
	/// queries simultaneously.
	/// </summary>
	/// <param name="camIdx">The camera ID to query for.</param>
	/// <returns>
	/// The search results. If not found, the returned Entry's cachedCamIdx will be -1.
	/// </returns>
	Entry GetInfoCopy(int camIdx);

	GLuint GetID(int camIdx);
	bool HasID(int camIdx);
	GLuint LoadTexture(int camIdx, cv::Ptr<cv::Mat> img, long long lastSeen);

	bool ClearTexture(int camIdx);
	void ClearTextures();
};