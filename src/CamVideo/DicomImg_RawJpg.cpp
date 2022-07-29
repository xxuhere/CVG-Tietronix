#include "DicomImg_RawJpg.h"
#include <opencv2/imgcodecs.hpp>

DicomImg_RawJpg::DicomImg_RawJpg(cv::Mat* matImg )
{
	this->matImg = matImg;
}

OFString DicomImg_RawJpg::inputFormat () const
{
	// Return the same thing as I2DJpegSource::inputformat().
	return "JPEG";
}

OFCondition DicomImg_RawJpg::readPixelData (
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
	std::vector<uchar> jpgBuf;
	cv::imencode(".jpg", *this->matImg, jpgBuf, std::vector<int>());

	// Output values. Some are taken from the OpenCV matrix, and some
	// are leveraging insight from DMCTK's I2DJpegSource::readPixelData().
	rows = this->matImg->rows;
	cols = this->matImg->cols;
	samplesPerPixel = this->matImg->channels();
	photoMetrInt = (this->matImg->channels() == 1) ? "MONOCHROME2" : "YBR_FULL_422";
	// Crossing my fingers that this is what is meant...
	// https://rawpedia.rawtherapee.com/Bit_Depth
	// "The very popular JPEG format typically saves images with a precision of 8 bits per channel"
	bitsAlloc = this->matImg->channels() * 8;
	bitsStored = bitsAlloc;
	highBit = bitsStored - 1;
	pixelRepr = 0;
	planConf = 0;
	pixAspectH = this->matImg->cols / this->matImg->rows;
	pixAspectV = this->matImg->rows / this->matImg->cols;
	length = jpgBuf.size(); // TODO: This may need to be an even number
	// This may need more accurate usage, see I2DJpegSource::associatedTS() 
	// for more info.
	ts = EXS_JPEGProcess1;

	pixData = new char[length];
	memcpy(pixData, &jpgBuf[0], length);

	return EC_Normal;
}

OFCondition DicomImg_RawJpg::getLossyComprInfo (
	OFBool &srcEncodingLossy, 
	OFString &srcLossyComprMethod) const
{
	// For now, always assuming OpenCV imencode is doing lossy compression.
	srcEncodingLossy = true;
	// "Taken from I2DJpegSource::getLossyComprInfo()
	srcLossyComprMethod = "ISO_10918_1";
	return EC_Normal;
}