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

// Read bytes from a position in array, to reinterpret short from
// big endian to little endian.
int ExtractWord(const uchar* bytes, int offset)
{
	uchar c1 = bytes[offset + 0];
	uchar c2 = bytes[offset + 1];
	return (c2 << 8) | c1;
}

// Read bytes from a position in array, to reinterpret int from
// big endian to little endian
int ExtractDoubleWord(const uchar* bytes, int offset)
{
	uchar c1 = bytes[offset + 0];
	uchar c2 = bytes[offset + 1];
	uchar c3 = bytes[offset + 2];
	uchar c4 = bytes[offset + 3];
	return 
		(c4 << 24) |
		(c3 << 16) |
		(c2 << 8 ) |
		(c1 << 0 );
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
	// We'll need a mono or 3 channel image to save to bitmap. Thenere's the question
	// of what to do if it's an 3 channel with an alpha...
	//
	// If it's an 3 (or 4) channel image, we'll need to convert the BGR format 
	// of OpenCV images to RGB. Keep in mind the images we are holding aren't 
	// literal pixel datas using to display in the CVG view (although 
	if(matImg->channels() == 4)
	{
		// If it got 4 channels, it's assumed to be a heatmap with an alpha mask.
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
		std::vector<cv::Mat> channelsReorder = 
		{
			channels[2], 
			channels[1], 
			channels[0], 
			channels[3]
		};
		// Save to new image. We were given matImg (via the constructor) from
		// some source which still holds ownership - so our 3 channel version
		// needs to be a copy and can't modify the original.
		cv::Ptr<cv::Mat> preMul(new cv::Mat());
		cv::merge(&channelsReorder[0], 3, *preMul);
		// Release the reference to the previous image, and use the new preMul-ed RGB
		this->matImg = preMul;
	}
	else if(matImg->channels() == 3)
	{
		std::vector<cv::Mat> channels(3);
		cv::split(*matImg, &channels[0]);
		std::vector<cv::Mat> channelsReorder = 
		{
			channels[2], 
			channels[1], 
			channels[0]
		};
		cv::Ptr<cv::Mat> reordered(new cv::Mat());
		cv::merge(&channelsReorder[0], 3, *reordered);
		this->matImg = reordered;
	}

	// BMPs save pixel rows flipped, but cv::imencode() for BMPs don't actually
	// flip it for us in the BMP binary it creates, so we need to do that manually.
	cv::Mat flipped;
	cv::flip(*this->matImg, flipped, 0);

	// https://doc.xuwenliang.com/docs/video_audio/3924 
	std::vector<uchar> bmpBuf;
	cv::imencode(".bmp", flipped, bmpBuf, std::vector<int>());

	// We don't actually package the entire BMP binary -
	// So we need to strip out the header. The lines for that below have been translated
	// from i2bmps.cc
	// 
	// We're not going to do anything too fancy with how many bitmap features we handle - 
	// as we're using OpenCV, and it's probably only going to do so many things - such as
	// NOT use color palettes.
	int dataOffset	= ExtractWord(&bmpBuf[0], 10);
	//int dataWidth	= ExtractDoubleWord(&bmpBuf[0], 18);
	//int dataHeight	= ExtractDoubleWord(&bmpBuf[0], 22);
	//int dataBPP		= ExtractWord(&bmpBuf[0], 28);


	const bool isMonochrome = flipped.channels() == 1;
	if(isMonochrome)
	{
		samplesPerPixel = flipped.channels();
		photoMetrInt = "MONOCHROME2";
	}
	else
	{
		cvgAssert(flipped.channels() == 3, "3 Channel RGB are the only non-monochrome BMP images supported");
		samplesPerPixel = 3;
		photoMetrInt = "RGB";
	}

	// Output values. Some are taken from the OpenCV matrix, and some
	// are leveraging insight from DMCTK's I2DBmpSource::readPixelData().
	rows			= flipped.rows;
	cols			= flipped.cols;
	bitsAlloc		= 8;
	bitsStored		= 8;
	highBit			= 7;
	planConf		= 0;
	pixAspectH		= 1;
	pixAspectV		= 1;
	pixelRepr		= 0;
	ts = EXS_LittleEndianExplicit;

	// From dataOffset, we're going to assume the rest is image data.
	length = bmpBuf.size() - dataOffset;
	pixData = new char[length];
	memcpy(pixData, &bmpBuf[dataOffset], length);

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