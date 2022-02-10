#pragma once
#include <memory>
#include <string>
#include <thread>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

class StreamMgr;
class StreamCon;

///
class StreamSession
{
private:

	/// <summary>
	/// Enums to command the StreamSession to attempt to reconnect
	/// the connection.
	/// </summary>
	enum class Recon
	{
		/// <summary>
		/// No reconnect command is set.
		/// </summary>
		Void,

		/// <summary>
		/// Only attempt to reconnect if not currently connected
		/// to anything.
		/// </summary>
		Soft,

		/// <summary>
		/// Force a reconnect, even if a valid connection is active.
		/// </summary>
		Hard
	};
private:
	StreamMgr * mgr;

	/// <summary>
	/// Is the session running? This represents both if the
	/// thread and stream connection is reliable.
	/// </summary>
	bool active;

	/// <summary>
	/// The canonicalized URL
	/// </summary>
	std::string streamURI;

	/// <summary>
	/// The background thread running the OpenCV polling of
	/// new frames from OpenCV - as well as dispatching notifications
	/// for the connections.
	/// </summary>
	std::thread processingThread;

	/// <summary>
	/// The OpenCV video stream.
	/// </summary>
	cv::VideoCapture stream;

	/// <summary>
	/// The width resolution of the video stream. 
	/// Only valid when the network connection is valid.
	/// </summary>
	int width		= -1;

	/// <summary>
	/// The height resolution of the video stream.
	/// Only valid when the network connection is valid.
	/// </summary>
	int height		= -1;

	char fourc[4];

	/// <summary>
	/// Mutex for ensuring thread saftey when reading/writing
	/// the connections.
	/// </summary>
	std::mutex connectionsMut;

	/// <summary>
	/// The list of all the connections referencing this session.
	/// </summary>
	std::vector<std::shared_ptr<StreamCon>> connections;

	Recon reconnectCmd = Recon::Void;

public:
	/// <summary>
	/// The most recent polled image from imageData. 
	/// 
	/// This could probably just be a normal pointer, but a shared
	/// pointer gives us a little more peice of mind of not having
	/// to manage it.
	/// </summary>
	std::shared_ptr<cv::Mat> imageData;

	/// <summary>
	/// Mutex to read or write to imageData
	/// </summary>
	std::mutex imageDataMut;
	/// <summary>
	/// If true, the next thing to encounter this session (that's able)
	/// should update the contents of imageData into glTextureID.
	/// 
	/// This should be thread protected via imageDataMut.
	/// </summary>
	bool textureDirty = true;

	/// <summary>
	/// The OpenGL texture for the current vide frame. It's 
	/// assumed the entire app will share the same OpenGL context.
	/// </summary>
	unsigned int glTextureID = (unsigned int)-1;

public:
	bool Active() 
	{return this->active;}

	std::string URI() const 
	{ return this->streamURI; }

	int Width() const
	{ return this->width; } 

	int Height() const
	{ return this->height; }

public:
	StreamSession(StreamMgr * parent, const std::string & uri);
	~StreamSession();

	typedef std::shared_ptr<StreamSession> Ptr;

	void Reconnect(bool force = false);

	/// <summary>
	/// Diconnect the network session.
	/// </summary>
	/// <param name="unreg">
	/// Unregister the session from the manager. This will be false
	/// for some internal calls where unregistration is handled elsewhere.
	/// For all other cases, leave as the defaulted true.
	/// </param>
	/// <param name="endthread">
	/// If true, end and join the thread.
	/// </param>
	void DisconnectSession(bool unreg = true, bool joinThread = true);

	bool Disconnect(StreamCon* con);

	// TODO: Properly encapsulate
	void _ThreadFunction();
	void _StartThread();

	std::shared_ptr<StreamCon> MakeConnection();
};