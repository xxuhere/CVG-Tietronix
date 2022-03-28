#include "TexObj.h"
#include "glext.h"

TexObj::TexObj()
{
}

TexObj::~TexObj()
{
	this->Destroy();
}

void TexObj::TransferFromCVMat(const cv::Mat& m)
{
	if(m.empty())
		return;

	if(this->texID == (GLuint)-1)
	{
		glGenTextures(1, &this->texID);
		glBindTexture(GL_TEXTURE_2D, this->texID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}

	glBindTexture(GL_TEXTURE_2D, this->texID);

	// Right now this is hardcoded to BGR. We should consider supporting
	// different formats, especially 1 channel greyscales.
	glTexImage2D(
		GL_TEXTURE_2D, 
		0, 
		GL_RGB, 
		m.cols, 
		m.rows, 
		0, 
		GL_BGR, 
		GL_UNSIGNED_BYTE,
		m.ptr());
}

void TexObj::TransferFromCVMat(const cv::Ptr<cv::Mat>& ptr)
{
	if(!ptr)
		return;

	this->TransferFromCVMat(*ptr);
}

void TexObj::GLBind()
{
	glBindTexture(GL_TEXTURE_2D, this->texID);
}

void TexObj::Destroy()
{
	if(!this->IsValid())
		return;

	glDeleteTextures(1, &this->texID);
	this->texID = (GLuint)-1;
}