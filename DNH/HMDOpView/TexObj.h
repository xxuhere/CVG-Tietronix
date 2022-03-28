#pragma once
#include <wx/glcanvas.h>
#include <opencv2/core.hpp>

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

	void GLBind();

	inline bool IsValid() 
	{ return this->texID != (GLuint)-1; }

	void Destroy();
};

