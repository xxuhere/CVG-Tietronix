#pragma once

#include "../DicomUtils/DicomInjector.h"
#include "../../Utils/VideoPollType.h"
#include "../../Utils/cvgOptions.h"
#include <opencv2/core.hpp>
#include <dcmtk/dcmdata/dcdeftag.h>

/// <summary>
/// Base class for an implementation of polling video frames from
/// a source (usually a web camera) for the HMDOpView to digest and
/// perform image processing on.
/// </summary>
class ICamImpl
{
protected:
	/// <summary>
	/// Check if the camera has called InitializeImple() yet.
	/// An ICamImpl should only be initialized once, upon its creation.
	/// </summary>
	bool hasInitialized = false;

	/// <summary>
	/// Check if the camera has been shutdown yet.
	/// </summary>
	bool hasShutdown = false;

	/// <summary>
	/// The preffered width to stream in data.
	/// </summary>
	int prefWidth = 0;

	/// <summary>
	/// The preffered height to stream in data.
	/// </summary>
	int prefHeight = 0;

	bool flipHoriz = false;

	bool flipVert = false;

protected:
	// Implementation methods.
	// THESE SHOULD -=#=>NEVER<=#=- BE CALLED DIRECTLY except by the
	// managing functions, which have the same name without
	// the "Impl" postfix.
	//
	// The only exception is within subclasses who have insider 
	// information on how exactly an implementation works.

	/// <summary>
	/// Subclass implementation for initializing.
	/// </summary>
	/// <returns>Returns false on error. Else, true</returns>
	virtual bool InitializeImpl() = 0;

	/// <summary>
	/// Subclass implementation for shutting down.
	/// </summary>
	/// <returns>Returns false on error. Else, true</returns>
	virtual bool ShutdownImpl() = 0;

	/// <summary>
	/// Subclass implementation for activating polling.
	/// </summary>
	/// <returns>Returns false on error. Else, true</returns>
	virtual bool ActivateImpl() = 0;

	/// <summary>
	/// Subclass implementation for deactivating polling.
	/// </summary>
	/// <returns>Returns false on error. Else, true</returns>
	virtual bool DeactivateImpl() = 0;

	/// <summary>
	/// Polling implementation for pulling the newest frame from
	/// the (active) ICamImpl.
	/// 
	/// Note that whether or not the class is derived off of
	/// CamImpl_OpenCVBase, the return value makes the OpenCV 
	/// library a dependency of this class and system.
	/// </summary>
	/// <returns>
	/// A shared pointer to the last polled frame.
	/// </returns>
	virtual cv::Ptr<cv::Mat> PollFrameImpl() = 0;

	/// <summary>
	/// Utility to flip an image in OpenCV based on the camera's
	/// flipHoriz and flipVert members.
	/// </summary>
	/// <param name="mat">The OpenCV matrix to flip.</param>
	void UtilToFlipMatInOpenCV(cv::Mat& mat);

public:
	inline bool HasInitialized() const
	{ return this->hasInitialized;}

	inline bool HasShutdown() const
	{ return this->hasShutdown; }

	inline cv::Ptr<cv::Mat> PollFrame()
	{ return this->PollFrameImpl(); }

public:
	/// <summary>
	/// Query the type of polling implementation of the ICamImpl subclass.
	/// </summary>
	virtual VideoPollType PollType() = 0;

	/// <summary>
	/// Query the ICamImpl subclass if it is in a state that's valid
	/// for polling video frames.
	/// </summary>
	virtual bool IsValid() = 0;

	/// <summary>
	/// Initialize the ICamImpl. 
	/// If the camera has already been initialized, the request will be
	/// ignored and false will be returned.
	/// </summary>
	/// <returns>The success of initialization.</returns>
	bool Initialize();

	/// <summary>
	/// Shutdown the ICamImpl.
	/// An ICamImpl should be shutdown before it is destroyed. 
	/// But note, once an implementation is destroyed, it is no 
	/// longer usable.
	/// </summary>
	/// <returns>The success of shutdown.</returns>
	bool Shutdown();

	/// <summary>
	/// Activate an ICamImpl for polling. 
	/// In order to succeed:
	/// - The ICamImpl must be initialized
	/// - The ICamImpl cannot be shutdown
	/// - The ICamImpl subclass implementation must succeed
	/// </summary>
	/// <returns>
	/// The success of activating the ICamImpl for polling.
	/// </returns>
	bool Activate();

	/// <summary>
	/// Deactivate a (currently active/polling) ICamImpl.
	/// </summary>
	/// <returns>The success of deactivating the ICamImpl.</returns>
	bool Deactivate();

	/// <summary>
	/// Load options for how the ICamImpl should behave.
	/// 
	/// Subclasses should also call their base class implementations.
	/// e.g. this->ICamImpl::PullOptions(opts);
	/// </summary>
	/// <param name="opts">A struct that contains camera behaviour
	/// options, that we originally pulled from the AppOptions.json.
	/// </param>
	/// <returns>The success of reading options.</returns>
	virtual bool PullOptions(const cvgCamFeedLocs& opts);

	/// <summary>
	/// This will be called from an IManagedCam - used to 
	/// 
	/// 
	/// NOTE THIS WILL NOT BE (currently) USED FOR COMPOSITES.
	/// </summary>
	/// <param name="dicomData"></param>
	virtual void DelegatedInjectIntoDicom(DcmDataset* dicomData);

	virtual ~ICamImpl();
};