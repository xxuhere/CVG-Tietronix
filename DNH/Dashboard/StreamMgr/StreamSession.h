#pragma once
#include <memory>
#include <string>
#include <thread>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

class StreamMgr;
class StreamCon;

/// <summary>
/// An object representing a streaming video feed to a specific
/// URI.
/// 
/// This uses OpenCV's FFMPEG implementation - the features and
/// behaviour is tied to that.
/// </summary>
// TODO: Make StreamSession have a private constructor, and make
// StreamMgr a friend, so only StreamMgr can create them. This will
// ensure the class's creation and usage is done strictly as designed.
class StreamSession
{
public:

	/// <summary>
	/// The various connection states of a session
	/// </summary>
	enum class ConState
	{
		Unknown,
		Disconnected,
		Connecting,
		Connected,
	};

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
		Hard,

		/// <summary>
		/// Keep the the stream and connections alive, but kill the
		/// network connection.
		/// </summary>
		Halt
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

	/// <summary>
	/// Character ID used to define the type of file format
	/// being streamed.
	/// 
	/// The value is provided from OpenCV. 
	/// See https://en.wikipedia.org/wiki/FourCC for more information.
	/// </summary>
	/// TODO: May not be implemented and tested.
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

	/// <summary>
	/// Command for reconnecting the StreamSession network stream.
	/// If a reset is requested, we don't reset the stream instantly or
	/// directly. Instead, the StreamSession will poll this value (which
	/// will be defaulted as Void to do nothing) and things can change
	/// that value to tell the thread to perform a refresh on the next
	/// poll cycle.
	/// </summary>
	Recon reconnectCmd = Recon::Void;

	/// <summary>
	/// Cache of the current connection state
	/// </summary>
	ConState conState = ConState::Disconnected;

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

	/// <summary>
	/// Request a stream reconnection.
	/// </summary>
	/// <param name="force">If false, the reconnection request will be
	/// ignored if the stream is already connected.</param>
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

	/// <summary>
	/// Disconnect a specific StreamCon.
	/// </summary>
	/// <param name="con">A connection that is owned by the StreamSession.</param>
	/// <returns>True if success.</returns>
	bool Disconnect(StreamCon* con);

	void Halt();

	/// <summary>
	/// The running thread function for StreamSession. It's in charge
	/// of handling the video stream and notifying StreamCons at
	/// regular intervals.
	/// 
	/// This function should not be called directly,
	/// </summary>
	/// TODO: Properly encapsulate
	void _ThreadFunction();

	/// <summary>
	/// Bootstrapping function for _ThreadFunction().
	/// </summary>
	void _StartThread();

	ConState ConnectionState() const
	{ return this->conState; }

	/// <summary>
	/// Create a new connection.
	/// This should be the only way to create StreamCon objects.
	/// </summary>
	/// <returns>A new StreamCon object.</returns>
	std::shared_ptr<StreamCon> MakeConnection();
};