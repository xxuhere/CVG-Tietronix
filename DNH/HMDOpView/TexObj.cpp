#include "TexObj.h"
#include "glext.h"
#include "lodePNG/lodepng.h"
#include <vector>
#include <iostream>

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

	if(this->IsValid())
		this->Destroy();

	if(this->texID == (GLuint)-1)
	{
		glGenTextures(1, &this->texID);
		glBindTexture(GL_TEXTURE_2D, this->texID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}

	glBindTexture(GL_TEXTURE_2D, this->texID);

	/*int srcType = GL_BGR;
	if(m.dims == 1)
		srcType = GL_RED;
	else if(m.dims == 2)
		srcType = GL_RG;
	else if(m.dims == 4)
		srcType = GL_BGRA;*/
	int internalFormat;
	int format;
	switch (m.channels())
	{
	case 1:
		internalFormat = GL_LUMINANCE;
		format = GL_LUMINANCE;
		break;
	case 2:
		internalFormat = GL_RG;
		format = GL_RG;
		break;
	case 3:
		internalFormat = GL_BGR;
		format = GL_RGB;
		break;
	case 4:
		internalFormat = GL_RGBA;
		format = GL_RGBA;
		break;

	}
	glTexImage2D(
		GL_TEXTURE_2D, 
		0, 
		internalFormat, 
		m.cols, 
		m.rows, 
		0, 
		format, 
		GL_UNSIGNED_BYTE,
		m.ptr());

	this->width = m.cols;
	this->height = m.rows;
}

void TexObj::TransferFromCVMat(const cv::Ptr<cv::Mat>& ptr)
{
	if(!ptr)
		return;

	this->TransferFromCVMat(*ptr);
}

bool TexObj::LoadFromImage(const std::string& imgFilepath)
{
	cv::Mat mat = cv::imread(imgFilepath);
	if(mat.empty())
		return false;

	this->TransferFromCVMat(mat);
	return true;
}

bool TexObj::LODEFromImage(const std::string& imgFilepath)
{
	std::vector<unsigned char> image;
	unsigned width, height;

	// https://raw.githubusercontent.com/lvandeve/lodepng/master/examples/example_decode.cpp
	unsigned error = lodepng::decode(image, width, height, imgFilepath.c_str());

	if(error)
	{
		std::cerr << "Could not load into lodePNG " << imgFilepath.c_str() <<std::endl;
		return false;
	}

	std::cout << "Loaded via lodePNG " << imgFilepath << " with width " << width << " and height " << height << std::endl;

	if(this->IsValid())
		this->Destroy();

	if(this->texID == (GLuint)-1)
	{
		glGenTextures(1, &this->texID);
		glBindTexture(GL_TEXTURE_2D, this->texID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}

	glBindTexture(GL_TEXTURE_2D, this->texID);

	glTexImage2D(
		GL_TEXTURE_2D, 
		0, 
		GL_RGBA8, 
		width, 
		height, 
		0, 
		GL_RGBA, 
		GL_UNSIGNED_BYTE,
		&image[0]);

	return true;
}

void TexObj::Destroy()
{
	if(!this->IsValid())
		return;

	glDeleteTextures(1, &this->texID);
	this->texID = (GLuint)-1;
}