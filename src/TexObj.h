#pragma once

#include <wx/glcanvas.h>
#include <opencv2/core.hpp>
#include <cmath>
#include <opencv2/imgcodecs.hpp>
#include <memory>

/// <summary>
/// Utility class to manage OpenGL textures as objects, as
/// well as any other related utilities for this app.
/// 
/// NOTES ON LODEPNG: OpenCV has a lot of image utilities, which
/// is why it's used so pervasively in the various system for
/// the image representation. But for loading PNGs, the LodePNG is
/// seen as more featured and reliable, especially for its form
/// factor (compared to far-more-bloated libraries like LibPNG, and
/// the complexity it would cost to integrate them).
/// </summary>
class TexObj
{
public:

	/// <summary>
	/// The id for the OpenGL texture.
	/// </summary>
	GLuint texID = (GLuint)-1;

	/// <summary>
	/// The cached image width.
	/// </summary>
	int width = -1;

	/// <summary>
	/// The cached image height.
	/// </summary>
	int height = -1;

	/// <summary>
	/// Return values for LODEIfEmpty()
	/// </summary>
	enum ELoadRet
	{
		/// <summary>
		/// Cannot load image, requested path is invalid.
		/// </summary>
		Invalid,

		/// <summary>
		/// Will not load image, TextObj already has image content.
		/// </summary>
		AlreadyLoaded,

		/// <summary>
		/// Requested image successfully loaded.
		/// </summary>
		Success
	};

public:
	TexObj();
	~TexObj();

	/// <summary>
	/// Transfer the image data from an OpenCV Mat to a TexObj.
	/// </summary>
	/// <param name="m">The OpenCV Mat to transfer.</param>
	void TransferFromCVMat(const cv::Mat& m);

	/// <summary>
	/// Transfer the image data from an OpenCV Mat cv::Ptr to a TexObj.
	/// </summary>
	/// <param name="ptr">The OpenCV Mat to transfer.</param>
	void TransferFromCVMat(const cv::Ptr<cv::Mat>& ptr);

	/// <summary>
	/// Load an image using OpenCV image loading code into the TexObj.
	/// </summary>
	/// <param name="imgFilepath">The filepath of the image to load.</param>
	/// <returns>True if the image was loaded successfuly, else false.</returns>
	bool LoadFromImage(const std::string& imgFilepath);

	/// <summary>
	/// Load an image using LodePNG into the TexObj.
	/// </summary>
	/// <param name="imgFilepath">The filepath of the PNG to load.</param>
	/// <returns>True if the image was loaded successfuly, else false.</returns>
	bool LODEFromImage(const std::string& imgFilepath);
	
	/// <summary>
	/// Load an image using LodePNG into the TexObj, BUT ONLY IF there
	/// is currently nothing loaded in the TexObj.
	/// 
	/// This is used as a lazy way to initialize a TexObj in code without
	/// branching with exterior code.
	/// </summary>
	/// <param name="imgFilepath">The filepath of the PNG to load.</param>
	/// <returns>The status of the request.</returns>
	ELoadRet LODEIfEmpty(const std::string& imgFilepath);

	/// <summary>
	/// Bind the OpenGL texture.
	/// Only valid if the image data isn't empty.
	/// </summary>
	inline void GLBind() const
	{ glBindTexture(GL_TEXTURE_2D, this->texID); }

	/// <summary>
	/// Calculate the vertical aspect ratio.
	/// </summary>
	/// <returns></returns>
	inline float VAspect()
	{
		if(this->width <= 0)
			return NAN;

		return (float)this->height / (float)this->width;
	}

	inline bool IsValid() const 
	{ return this->texID != (GLuint)-1; }

	/// <summary>
	/// Release the OpenGL texture data.
	/// </summary>
	void Destroy();

	typedef std::shared_ptr<TexObj> SPtr;

	/// <summary>
	/// Load an image file as a TexObj::SPtr using OpenCV.
	/// </summary>
	/// <param name="imgFilepath">The filepath of the image to load.</param>
	/// <returns>The loaded image object; or null if loading was not successful.</returns>
	static SPtr MakeSharedLoad(const std::string& imgFilepath);

	/// <summary>
	/// Load a png file as a TexObj::SPtr using the LodePNG library.
	/// </summary>
	/// <param name="imgFilepath">The filepath of the PNG image to load.</param>
	/// <returns>The loaded image object; or null if loading was not successful.</returns>
	static SPtr MakeSharedLODE(const std::string& imgFilepath);
};

