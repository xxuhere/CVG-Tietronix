#include <dcmtk/dcmdata/libi2d/i2dimgs.h>
#include <opencv2/core.hpp>

// See i2djpgs.cc from the DMCTK library for the template of how
// jpegs are saved to dicoms.

/// <summary>
/// Implementation of I2DImgSource to save an OpenCV image as a
/// jpeg, embedded in a Dicom file.
/// </summary>
class DicomImg_RawJpg : public I2DImgSource
{
public:
	/// <summary>
	/// The image to jpeg encode into a Dicom.
	/// </summary>
	cv::Mat* matImg;

public:
	DicomImg_RawJpg(cv::Mat* matImg );

	OFString inputFormat () const override;

	OFCondition readPixelData (
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
		E_TransferSyntax &ts) override;

	OFCondition getLossyComprInfo(
		OFBool &srcEncodingLossy, 
		OFString &srcLossyComprMethod) const override;
};