#include "IManagedCam.h"
#include "ManagedComposite.h"

#include <opencv2/imgcodecs.hpp>
#include "../Utils/multiplatform.h"
#include "../Utils/cvgStopwatch.h"
#include "../Utils/cvgStopwatchLeft.h"

#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/libi2d/i2dimgs.h>
#include <dcmtk/dcmdata/libi2d/i2dbmps.h>
#include <dcmtk/dcmdata/libi2d/i2djpgs.h>
#include <dcmtk/dcmdata/libi2d/i2d.h>
#include <dcmtk/dcmdata/dcdict.h>
#include <dcmtk/ofstd/ofcmdln.h>
#include <dcmtk/dcmdata/libi2d/i2dplsc.h>


#include "DicomImg_RawJpg.h"

#include "../DicomUtils/DicomInjectorSet.h"
#include "../DicomUtils/DicomMiscUtils.h"

#include <iostream>
#include "../Utils/cvgAssert.h"


float IManagedCam::GetFloat( StreamParams paramid)
{
	switch(paramid)
	{
	case StreamParams::Alpha:
		return this->alpha;
	}

	return 0.0f;
}

bool IManagedCam::SetFloat( StreamParams paramid, float value)
{
	switch(paramid)
	{
	case StreamParams::Alpha:
		this->alpha = value;
		return true;
	}

	return false;
}

bool IManagedCam::ShutdownThread()
{
	if(this->camStreamThread == nullptr)
		return true;

	// Send the signal for the thread the shut down. Then it's up to
	// to the thread to see the signal and shut itself down.
	this->_sentShutdown = true;
	return true;
}

bool IManagedCam::_JoinThread()
{
	if(this->camStreamThread == nullptr || !this->_isShutdown)
		return false;

	this->camStreamThread->join();
	delete this->camStreamThread;
	this->camStreamThread = nullptr;
	return true;
}

cv::Ptr<cv::Mat> IManagedCam::GetCurrentFrame()
{
	// Get a copy of the shared ptr in a controlled manner, when we
	// know it's stable.
	cv::Ptr<cv::Mat> cpyRet;

	{ // Mutex guard scope
		std::lock_guard<std::mutex> guard(this->imageAccess);
		cpyRet = this->curCamFrame;
	}
	return cpyRet;
}

bool IManagedCam::SetCurrentFrame(cv::Ptr<cv::Mat> mat)
{
	std::lock_guard<std::mutex> guard(this->imageAccess);
	this->curCamFrame = mat;
	++this->camFeedChanges;
	return true;
}

SnapRequest::SPtr IManagedCam::RequestSnapshot(
	const std::string& filename, 
	SnapRequest::ProcessType procType)
{
	SnapRequest::SPtr req = SnapRequest::MakeRequest(filename, procType);
	{
		// Thread protected add to the to-process list.
		std::lock_guard<std::mutex> guard(this->snapReqsAccess);
		req->status = SnapRequest::Status::Requested;
		this->snapReqs.push_back(req);
	}
	return req;
}

void IManagedCam::ClearSnapshotRequests()
{
	std::vector<SnapRequest::SPtr> swapDst;
	{
		std::lock_guard<std::mutex> guard(this->snapReqsAccess);
		std::swap(swapDst, this->snapReqs);
	}

	for(SnapRequest::SPtr s : swapDst)
	{
		// Technically not an error, but a cancellation. But, this
		// will do.
		s->status = SnapRequest::Status::Error;
		s->err = "Requested snapshots were cleared.";
	}
}

bool IManagedCam::_CloseVideo_NoMutex()
{
	if(this->videoWrite.isOpened())
		this->videoWrite.release();

	if(this->activeVideoReq == nullptr)
		return false;

	this->activeVideoReq->status = VideoRequest::Status::Closed;
	this->activeVideoReq = nullptr;

	return true;
}

bool IManagedCam::_DumpImageToVideofile(const cv::Mat& img)
{
	if(img.empty())
	{
		this->_CloseVideo_NoMutex();
		return false;
	}

	// No request of where to save it was set.
	if(this->activeVideoReq == nullptr)
		return false;

	// If the user requested the stream to be stopped, using the
	// request handle (via VideoRequest::RequestStop()).
	if(this->activeVideoReq->_reqStopped)
	{ 
		this->_CloseVideo_NoMutex();

		// Technically... it was handled properly... so I guess it's true.
		return true;
	}

	const int msFor30FPS = 33; // 1000 / 30

	// If the videowrite has not been opened yet, open it locked
	// to the image size. Every image streamed to the video will
	// now need to match the dimensions, or else it's considered
	// an error.
	if(!this->videoWrite.isOpened())
	{
		int mp4FourCC = cv::VideoWriter::fourcc('a', 'v', 'c', '1');
		this->videoWrite.open(this->activeVideoReq->filename, mp4FourCC, 30.0, img.size());
		if(!this->videoWrite.isOpened())
		{
			this->activeVideoReq->err = "Could not open requested file.";
			this->activeVideoReq->status = VideoRequest::Status::Error;
			this->_CloseVideo_NoMutex();
			return false;
		}
		this->activeVideoReq->width		= img.size().width;
		this->activeVideoReq->height	= img.size().height;
		this->activeVideoReq->status	= VideoRequest::Status::StreamingOut;

		this->videoGrabTimer.Reset(msFor30FPS);
	}
	
	// Safeguard for how much we pad. In the worst case scenario, 
	// the device will be so slow that by the time it writes a frame, 
	// the next 33ms have passed and we'll be busy writing video 
	// padding for this single frame forever.
	const int maxFramePadding = 10;
	int i = 0;
	while(this->videoGrabTimer.GrabMS(msFor30FPS) && i < maxFramePadding)
	{
		// If contents have already been streamed, just make sure the dimensions
		// continue to be the same.
		if(
			this->activeVideoReq->width		!= img.size().width ||
			this->activeVideoReq->height	!= img.size().height )
		{
			this->activeVideoReq->err = "Closed when attempting to add misshapened image.";
			this->activeVideoReq->status = VideoRequest::Status::Error;
			this->_CloseVideo_NoMutex();
			break;
		}

		this->videoWrite.write(img);
		++i;
	}
	return true;
}

bool IManagedCam::CloseVideo()
{
	std::lock_guard<std::mutex> guard(this->videoAccess);
	return this->_CloseVideo_NoMutex();
}

bool IManagedCam::IsRecordingVideo()
{
	std::lock_guard<std::mutex> guard(this->videoAccess);
	return this->videoWrite.isOpened();
}

std::string IManagedCam::VideoFilepath()
{
	std::lock_guard<std::mutex> guard(this->videoAccess);
	if(this->activeVideoReq == nullptr)
		return std::string();

	return this->activeVideoReq->filename;
}

/// <summary>
/// Save an OpenCV mat as a jpeg in a Dicom file.
/// </summary>
/// <param name="imgMat">The image to save.</param>
/// <param name="cam">The camera containing extra camera Dicom data.</param>
/// <param name="baseFilename">The filename to save the Dicom file as.</param>
/// <returns>true if successful, else false.</returns>
bool SaveMatAsDicomJpeg(cv::Ptr<cv::Mat> imgMat, IManagedCam* cam, const std::string& baseFilename)
{
	//////////////////////////////////////////////////
	//	TEMPORARY CODE FOR DICOM
	//
	// We will eventually need to fix several issues:
	// - Dicom filenames cannot be longer than 8 characters, including the ".dcm"
	//	- Or at least by spec they shouldn't be.
	// - Location of DicomImg_RawJpg needs to be confirmed.
	// - Whether we do this raw in this location, or delegate to a formal system
	//	should be thought-out.

	DicomInjectorSet& dicomInjSet = DicomInjectorSet::GetSingleton();

	Image2Dcm i2d;
	i2d.setValidityChecking(OFTrue, OFTrue, OFTrue); // using default param values from img2dcm DCMTK sample

	I2DOutputPlug* outPlug = new I2DOutputPlugSC();
	outPlug->setValidityChecking(OFTrue, OFTrue, OFTrue);

	if (!dcmDataDict.isDictionaryLoaded())
	{
		std::cerr << 
			"no data dictionary loaded, check environment variable: "
			<< DCM_DICT_ENVIRONMENT_VARIABLE 
			<< std::endl;
	}

	I2DImgSource* inputImgSrc = new DicomImg_RawJpg(imgMat.get());
	inputImgSrc->setImageFile("snapshot"); // TODO: Better name
	DcmDataset *resultObject = nullptr;
	E_TransferSyntax writeXfer;

	// !TODO: Error checking
	OFCondition cond = i2d.convertFirstFrame(inputImgSrc, outPlug, 1, resultObject, writeXfer);
	//
	cond = i2d.updateOffsetTable();
	cond = i2d.updateLossyCompressionInfo(inputImgSrc, 1, resultObject);
	DcmFileFormat dcmff(resultObject);

	DcmDataset* dicomData = dcmff.getDataset();
	//
	// ADD DICOM TIMESTAMPS
	// https://dicom.innolitics.com/ciods/cr-image/general-study/00080020
	dicomData->putAndInsertString(DCM_StudyDate, DicomMiscUtils::GetCurrentDateString().c_str());
	// https://dicom.innolitics.com/ciods/cr-image/general-study/00080030
	dicomData->putAndInsertString(DCM_StudyTime, DicomMiscUtils::GetCurrentTime().c_str());
	//
	// ADD DICOM INJECTION SET DATA
	dicomInjSet.InjectDataInto(dicomData);
	//
	// ADD DICOM CAMERA DATA
	cam->InjectIntoDicom(dicomData);

	cond = dcmff.saveFile((baseFilename + ".dcm").c_str(), writeXfer);

	delete inputImgSrc;
	delete outPlug;
	delete resultObject;

	return true;
}

bool IManagedCam::SaveMatAsDicomJpeg_HandleReq(
	cv::Ptr<cv::Mat> imgMat, 
	IManagedCam* cam, 
	SnapRequest::SPtr snreq,
	int camFeedChanges)
{
	// Snapshots can be cancelled from outside.
	if(snreq->status == SnapRequest::Status::Error)
		return false;

	// Save and notify success.
	if(SaveMatAsDicomJpeg(imgMat, cam, snreq->filename))
	{
		snreq->frameID = camFeedChanges;
		snreq->status = SnapRequest::Status::Filled;
		return true;
	}
	
	// Else, set error state
	//
	// Not the most in-depth error message, but using OpenCV
	// this way doesn't give us too much grainularity.
	snreq->err = "Error attempting to save file.";
	snreq->status = SnapRequest::Status::Error;
	return false;
}

bool IManagedCam::_FinalizeHandlingPolledImage(cv::Ptr<cv::Mat> ptr)
{
	if(ptr == nullptr)
		return false;

	if(ptr->empty())
		return false;

	bool processing = this->UsesImageProcessingChain();

	std::vector<SnapRequest::SPtr> sptrSwap;
	{
		std::lock_guard<std::mutex> guardReqs(this->snapReqsAccess);
		std::swap(sptrSwap, this->snapReqs);
	}

	//		PROCESS AND CACHE VIDEO
	// 
	//////////////////////////////////////////////////

	// Gather requests of raw snapshots.
	std::vector<SnapRequest::SPtr> rawSnaps;
	for(int i = 0; i < sptrSwap.size(); )
	{
		SnapRequest::SPtr snreq = sptrSwap[i];

		// Snapshots can be cancelled from outside.
		if(snreq->status == SnapRequest::Status::Error)
		{ 
			++i;
			continue;
		}

		if(
			snreq->processType == SnapRequest::ProcessType::HasTo  || 
			(snreq->processType == SnapRequest::ProcessType::Indifferent && processing))
		{
			++i;
			continue;
		}
		rawSnaps.push_back(sptrSwap[i]);
		sptrSwap.erase(sptrSwap.begin() + i);
	}

	// SAVE SNAPSHOTS OF PREPROCESSED
	//
	if(!rawSnaps.empty())
	{
		cv::Ptr<cv::Mat> saveMat = ptr;
		if(!this->snapCaption.empty())
		{
			saveMat = new cv::Mat();
			ptr->copyTo(*saveMat);

			ApplySnapshotWatermarkText(*saveMat, this->snapCaption);
		}

		for(SnapRequest::SPtr snreq : rawSnaps)
		{	
			SaveMatAsDicomJpeg_HandleReq(
				saveMat, 
				this, 
				snreq, 
				camFeedChanges);
		}
	}


	ptr = this->ProcessImage(ptr);

	if(ptr)
		this->SetCurrentFrame(ptr);

	// If a ManagedCam subclass, give the composite system a copy
	// of the frame.
	if(this->GetCamType() == CamType::VideoFeed)
	{
		ManagedComposite::CacheCameraFrame(
			this->GetID(), 
			ptr,
			this->UsesImageProcessingChain(),
			this->GetFloat(StreamParams::Alpha));
	}

	// SAVE SNAPSHOTS OF IMAGE PROCESSED
	//
	// Everything left in sptrSwap should be things that need to be
	// processed, or indifferents, if we're processing.
	if(!sptrSwap.empty())
	{ 
		// If we're saving with text, we need a seperate image with the text,
		// without polluting the original. Note that this will incur the cost
		// of a deep copy.
		cv::Ptr<cv::Mat> saveMat = ptr;
		if(!this->snapCaption.empty())
		{
			saveMat = new cv::Mat();
			ptr->copyTo(*saveMat);

			cv::putText(
				*saveMat, 
				this->snapCaption.c_str(), 
				cv::Point(20, ptr->rows - 30), 
				20, 
				1.0, 
				0xFF00FF);
		}


		// Attempt to save file and report the success status back to 
		// the shared pointer.
		for(SnapRequest::SPtr snreq : sptrSwap)
		{
			

			SaveMatAsDicomJpeg_HandleReq(
				saveMat, 
				this, 
				snreq,
				this->camFeedChanges);
		}
	}

	{
		//		SAVE VIDEO
		//
		//////////////////////////////////////////////////
		// If we're holding on to a non-null video request, it will be 
		// either waiting to stream, or already streaming - no misc or
		// error conditions.
		std::lock_guard<std::mutex> guardVideo(this->videoAccess);
		if(this->activeVideoReq != nullptr)
		{
			// Placing the watermark. This may not be the best location to 
			// do this, but it should be sufficient for now. Some things to
			// consider,
			// - If we're adding padding, we don't want to add in the padding region
			// - If we're reusing images, we don't want to repeatedly add to the
			//   same image.
			// - If we're not saving to video, we don't want to add text to frames
			//   that aren't doing to be saved to video.
			//
			// Also note that this may cause issues, watermark info
			// directly to the image instead of to a copy, but we'll try to
			// get away with this if we can, as encoding a video is already
			// expensive enough as-is without an additional image copy.
			if(!this->snapCaption.empty())
				ApplySnapshotWatermarkText(*ptr, this->snapCaption);

			this->_DumpImageToVideofile(*ptr);
		}
	}

	return true;
}

bool IManagedCam::BootupPollingThread(int camIdx)
{
	// Once someone shuts down the camera system for the app,
	// for the given app session, that's it- it's kaputskies 
	// (by design) for the rest of the lifetime's app.
	if(this->_sentShutdown)
		return false;

	// If the camera stream is already running, there's nothing that
	// need booting.
	if(this->camStreamThread != nullptr)
		return false;

	this->camStreamThread = 
		new std::thread(
			[this, camIdx]
			{
				// This thread is expected to run until 
				// CamStreamMgr::Shutdown() is called at the end
				// of the app's lifetime.

				// Flag the command to start the video capturing for the thread
				this->ThreadFn(camIdx);
				this->_isShutdown = true;
			});

	return true;
}

void IManagedCam::SetSnapCaption(const std::string& caption)
{
	this->snapCaption = caption;
}

VideoRequest::SPtr IManagedCam::OpenVideo(const std::string& filename)
{
	std::lock_guard<std::mutex> guard(this->videoAccess);

	if (filename.empty())
	{
		this->_CloseVideo_NoMutex();
		VideoRequest::SPtr noneRet = VideoRequest::MakeRequest(0, 0, this->GetID(), filename);
		noneRet->err = "Empty filename";
		noneRet->status = VideoRequest::Status::Error;
		return noneRet;
	}

	// If we're already recording the target, just sent back the
	// current active request.
	if(this->activeVideoReq != nullptr)
	{
		if(
			this->activeVideoReq->filename == filename && 
			this->activeVideoReq->status == VideoRequest::Status::StreamingOut)
		{
			return this->activeVideoReq;
		}
	}

	this->_CloseVideo_NoMutex();

	// We may have a frame immediately write, but we'll open it
	// tenatively first.
	this->activeVideoReq = VideoRequest::MakeRequest(0, 0, this->GetID(), filename);
	this->activeVideoReq->status = VideoRequest::Status::Requested;
	{
		// If we have a frame, that will set the size parameters.
		std::lock_guard<std::mutex> imgGuard(imageAccess);
		if(!curCamFrame.empty() && !curCamFrame->empty())
			this->_DumpImageToVideofile(*this->curCamFrame);
	}
	return this->activeVideoReq;
}

void IManagedCam::_DeactivateStreamState(bool deactivateShould)
{
	this->_isStreamActive		= false;
	this->streamWidth			= -1;
	this->streamHeight			= -1;
	this->msInterval			= 0;

	if(this->IsRecordingVideo())
		this->CloseVideo();
}

void IManagedCam::_EndShutdown()
{}

void IManagedCam::ApplySnapshotWatermarkText(
	cv::Mat& mat, 
	const std::string& text)
{
	cv::putText(
		mat, 
		text.c_str(), 
		cv::Point(20, mat.rows - 30), 
		20, 
		1.0, 
		0xFF00FF);
}

void IManagedCam::InjectIntoDicom(DcmDataset* dicomData)
{
	// Do-nothing, see declaration for more details.
}