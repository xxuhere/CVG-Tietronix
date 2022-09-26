#include "DicomImg_RawBmp.h"
#include <opencv2/imgcodecs.hpp>
#include "../Utils/cvgAssert.h"

DicomImg_RawBmp::DicomImg_RawBmp(cv::Mat* matImg )
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