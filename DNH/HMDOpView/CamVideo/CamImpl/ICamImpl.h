#pragma once
#include "../../Utils/VideoPollType.h"
#include "../../Utils/cvgOptions.h"
#include <opencv2/core.hpp>

class ICamImpl
{
protected:
	// State tracking variables that are common in all camera
	// implementations.
	bool hasInitialized = false;
	bool initStatus = false;
	bool hasShutdown = false;

protected:
	// Implementation methods.
	// THESE SHOULD -=#=>NEVER<=#=- BE CALLED DIRECTLY except by the
	// managing functions, which have the same name without
	// the "Impl" postfix.
	//
	// The only exception is within subclasses who have insider 
	// information on how exactly an implementation works.

	virtual bool InitializeImpl() = 0;
	virtual bool ShutdownImpl() = 0;
	virtual bool ActivateImpl() = 0;
	virtual bool DeactivateImpl() = 0;
	virtual cv::Ptr<cv::Mat> PollFrameImpl() = 0;

public:
	inline bool HasInitialized() const
	{ return this->hasInitialized;}

	inline bool HasShutdown() const
	{ return this->hasShutdown; }

	inline cv::Ptr<cv::Mat> PollFrame()
	{ return this->PollFrameImpl(); }

public:
	virtual VideoPollType PollType() = 0;
	virtual bool IsValid() = 0;

	bool Initialize();
	bool Shutdown();
	bool Activate();
	bool Deactivate();

	virtual bool PullOptions(const cvgCamFeedLocs& opts);

	virtual ~ICamImpl();
};