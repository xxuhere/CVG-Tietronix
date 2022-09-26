#include "DicomImg_RawBmp.h"
#include <opencv2/imgcodecs.hpp>
#include "../Utils/cvgAssert.h"

DicomImg_RawBmp::DicomImg_RawBmp(cv::Ptr<cv::Mat> matImg )
{
	this->matImg = matImg;
}

OFString DicomImg_RawBmp::inputFormat () const
{
	// Return the same thing as I2DBmpSource::inputformat().
	return "BMP";
}

OFCondition DicomImg_RawBmp::readPixelData (
	Uint16 &rows, 
	Uint16 &cols, 
	Uint16 &samplesPerPixel, 
	OFString &photoMetrInt, 
	Uint16 &bitsAlloc, 
	Uint16 &bitsStored, 
	Uint16 &highBit, 
	Uint16 &pixelRepr, 
	Uint16 &planConf, 
	Uint16 &pixAspectH, 
	Uint16 &pixAspectV, 
	char *&pixData, 
	Uint32 &length, 
	E_TransferSyntax &ts)
{
	// We'll need a mono or RGB image to save to bitmap. Thenere's the question
	// of what to do if it's an RGB with an alpha...
	if(matImg->channels() == 4)
	{
		// If it's a RGBA, it's assumed to be a heatmap with an alpha mask.
		// For now we'll assume it's more useful to show the heatmap with the
		// alpha already applied - since we can't also store the alpha channel
		// into a bitmap.

		// Something similar is also being done when rendering to OpenGL in StateHMDOp, 
		// we may just want to unify this in a utility function.

		std::vector<cv::Mat> channels(4);
		cv::split(*matImg, &channels[0]);
		// Apply alpha
		cv::multiply(channels[0], channels[3], channels[0], 1.0/255.0);
		cv::multiply(channels[1], channels[3], channels[1], 1.0/255.0);
		cv::multiply(channels[2], channels[3], channels[2], 1.0/255.0);
		// Save to new image. We were given matImg (via the constructor) from
		// some source which still holds ownership - so our 3 channel version
		// needs to be a copy and can't modify the original.
		cv::Ptr<cv::Mat> preMul(new cv::Mat());
		cv::merge(&channels[0], 3, *preMul);
		// Release the reference to the previous image, and use the new preMul-ed RGB
		this->matImg = preMul;
	}

	// https://doc.xuwenliang.com/docs/video_audio/3924 
	std::vector<uchar> bmpBuf;
	cv::imencode(".bmp", *this->matImg, bmpBuf, std::vector<int>());


	const bool isMonochrome = this->matImg->channels() == 1;
	if(isMonochrome)
	{
		samplesPerPixel = this->matImg->channels();
		photoMetrInt = "MONOCHROME2";
	}
	else
	{
		cvgAssert(this->matImg->channels() == 3, "3 Channel RGB are the only non-monochrome BMP images supported");
		samplesPerPixel = 3;
		photoMetrInt = "RGB";
	}

	// Output values. Some are taken from the OpenCV matrix, and some
	// are leveraging insight from DMCTK's I2DBmpSource::readPixelData().
	rows = this->matImg->rows;
	cols = this->matImg->cols;
	bitsAlloc		= 8;
	bitsStored		= 8;
	highBit			= 7;
	planConf		= 0;
	pixAspectH		= 1;
	pixAspectV		= 1;
	pixelRepr		= 0;
	ts = EXS_LittleEndianExplicit;

	length = bmpBuf.size(); // TODO: This may need to be an even number
	pixData = new char[length];
	memcpy(pixData, &bmpBuf[0], length);

	return EC_Normal;
}

OFCondition DicomImg_RawBmp::getLossyComprInfo (
	OFBool &srcEncodingLossy, 
	OFString &srcLossyComprMethod) const
{
	
	srcEncodingLossy = false;
	srcLossyComprMethod = "";
	return EC_Normal;
}