#pragma once

#include "ICamImpl.h"
#include <opencv2/videoio.hpp>
#include "../../Utils/cvgAssert.h"

/// <summary>
/// A base class for all ICamImpl implementations that will
/// use OpenCV. For the most part, their entire polling
/// process follows the same pattern and uses the same
/// code. The only real difference is how the initial
/// cv::VideoCapture is created.
/// </summary>
class CamImpl_OpenCVBase : public ICamImpl
{
private:
	/// <summary>
	/// The cv::VideoCapture for subclasses to allocate and
	/// initialize.
	/// </summary>
	cv::VideoCapture* ocvStream = nullptr;

protected:
	bool InitializeImpl() override;
	bool ShutdownImpl() override;
	bool ActivateImpl() override;
	bool DeactivateImpl() override;
	cv::Ptr<cv::Mat> PollFrameImpl() override;

	/// <summary>
	/// Subclasses will initialize the cv::VideoCapture used for
	/// polling, here.
	/// </summary>
	/// <returns>
	/// The created video capture.
	/// </returns>
	virtual cv::VideoCapture * CreateVideoCapture() = 0;

	/// <summary>
	/// Code for shared initialization of the VideoCapture.
	/// i.e., After CreateVideoCapture() is called for subclasses to
	/// create the VideoCapture however they need to, all created
	/// cv::VideoCapture will have InitCapture() called on them.k'
	/// </summary>
	/// <param name="capture">The VideoCapture to perform shared initialization on.</param>
	/// <returns>
	/// True if successful; else there was an issue with the capture parameter,
	/// or issue when attempting to initialize it.
	/// </returns>
	virtual bool InitCapture(cv::VideoCapture* capture);

	/// <summary>
	/// Calls InitCapture() on the internal owned VideoCapture.
	/// </summary>
	void InitCapture();

	inline bool IsStreamAllocated()
	{ return this->ocvStream != nullptr; }

	inline void AssertStreamNull()
	{ cvgAssert(!this->IsStreamAllocated(),"AssertStreamNull failed"); }

public:
	// No poll type here, that's the responsibility for 
	// subclasses (that use OpenCV) to specify.
	//
	//VideoPollType PollType() override;
	bool IsValid() override;

	void DelegatedInjectIntoDicom(DcmDataset* dicomData) override;
};