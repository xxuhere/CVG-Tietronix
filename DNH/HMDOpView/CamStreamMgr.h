#pragma once

#include "CamStreamMgr.h"
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

#include <mutex>
#include <thread>
#include <memory>

class CamStreamMgr
{
public:
	enum class State
	{
		Unknown,
		Idling,
		Connecting,
		Polling,
		Shutdown
	};

private:
	/// <summary>
	/// Singleton instance.
	/// </summary>
	static CamStreamMgr _inst;

public:
	/// <summary>
	/// Public accessor to singleton instance.
	/// </summary>
	static CamStreamMgr& GetInstance();

	/// <summary>
	/// This should be called at the end of the application's
	/// lifetime to properly shutdown the singleton instance.
	/// </summary>
	/// <returns>Success value. This can be ignored.</returns>
	static bool ShutdownMgr();

public:
	cv::VideoCapture stream;

	/// <summary>
	/// 
	/// </summary>
	std::mutex imageAccess;

	/// <summary>
	/// 
	/// </summary>
	cv::Ptr<cv::Mat> curCamFrame;

	/// <summary>
	/// 
	/// </summary>
	bool _sentShutdown = false;

	/// <summary>
	/// 
	/// </summary>
	bool _isShutdown = false;

	/// <summary>
	/// 
	/// </summary>
	bool _shouldStreamBeActive = false;

	/// <summary>
	/// 
	/// </summary>
	bool _isStreamActive = false;

	/// <summary>
	/// 
	/// </summary>
	std::thread* camStreamThread = nullptr;

	/// <summary>
	/// This should match with the number of times curCamFrame has been updated,
	/// so we if need track of the last time we polled curCamFrame and the 
	/// change counter, we know if we can expect a change if we poll again.
	/// 
	/// For simplicity, and because there's little threading danger doing so,
	/// this isn't going to be mutex guarded.
	/// </summary>
	long long camFeedChanges = 0;

	/// <summary>
	/// Cached width dimension of the streaming video frame.
	/// </summary>
	int streamWidth = -1;

	/// <summary>
	/// Cached height dimension of the streaming video frame.
	/// </summary>
	int streamHeight = -1;

	State conState = State::Unknown;

public:
	~CamStreamMgr();

	/// <summary>
	/// The thread loop to poll camera frames. This is expected to run in the 
	/// background, once booted. Afterwards, it will only end after the app
	/// is closed.
	/// </summary>
	void ThreadFn();

	/// <summary>
	/// 
	/// </summary>
	/// <param name="inImg"></param>
	/// <returns></returns>
	cv::Ptr<cv::Mat> ProcessImage(cv::Ptr<cv::Mat> inImg);

	/// <summary>
	/// 
	/// </summary>
	void BootConnectionToCamera();

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	cv::Ptr<cv::Mat> GetCurrentFrame();

	/// <summary>
	/// 
	/// </summary>
	/// <param name="mat"></param>
	/// <returns></returns>
	bool SetCurrentFrame(cv::Ptr<cv::Mat> mat);

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	bool Shutdown();

	inline State GetState() { return this->conState; }

private:
	CamStreamMgr();
};