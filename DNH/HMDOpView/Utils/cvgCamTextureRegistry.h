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
	/// <summary>
	/// A mapping between camera ids (as used for CamStreamMgr)
	/// and the last video frame.
	/// </summary>
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

	/// <summary>
	/// Get the OpenGL texture id for a camera id.
	/// </summary>
	/// <param name="camIdx">The camera id to retrive the texture id for.</param>
	/// <returns>The texture id, or (GLuint)-1 if none was found.</returns>
	GLuint GetID(int camIdx);

	/// <summary>
	/// Check if the registry has an entry for a camera id.
	/// </summary>
	/// <param name="camIdx">The camera id to check.</param>
	bool HasID(int camIdx);

	/// <summary>
	/// Set the texture for a camera id.
	/// </summary>
	/// <param name="camIdx">
	/// The id to store the image under.
	/// </param>
	/// <param name="img">
	/// The image to store as the camera's last known frame.
	/// </param>
	/// <param name="lastSeen">
	/// A unique value for the frame. Every unique frame should have a different
	/// value. A counter system should be sufficient. This value is used to
	/// drive a dirty tracking system - so things can track via the id if the
	/// frame has changed from a previous moment by checking the id the frame
	/// was last seen with, and the current lastSeen value.
	/// </param>
	/// <returns></returns>
	GLuint LoadTexture(int camIdx, cv::Ptr<cv::Mat> img, long long lastSeen);

	/// <summary>
	/// Clear the texture for a specific camera id.
	/// </summary>
	/// <param name="camIdx">The id to clear for.</param>
	/// <returns>
	/// If there was anything to clear, true; else, false.
	/// </returns>
	bool ClearTexture(int camIdx);

	/// <summary>
	/// Clear all textures in the system.
	/// </summary>
	void ClearTextures();
};