#pragma once
#include <wx/glcanvas.h>
#include <opencv2/core.hpp>
#include <cmath>
#include <opencv2/imgcodecs.hpp>

/// <summary>
/// Utility class to manage OpenGL textures as objects, as
/// well as any other related utilities for this app.
/// </summary>
class TexObj
{
public:

	GLuint texID = (GLuint)-1;
	int width = -1;
	int height = -1;

public:
	TexObj();
	~TexObj();

	void TransferFromCVMat(const cv::Mat& m);
	void TransferFromCVMat(const cv::Ptr<cv::Mat>& ptr);

	bool LoadFromImage(const std::string& imgFilepath);
	bool LODEFromImage(const std::string& imgFilepath);

	inline void GLBind()
	{ glBindTexture(GL_TEXTURE_2D, this->texID); }

	inline float VAspect()
	{
		if(this->width == 0)
			return NAN;

		return (float)this->height / (float)this->width;
	}

	inline bool IsValid() 
	{ return this->texID != (GLuint)-1; }

	void Destroy();
};

