#pragma once

#include <memory>
#include <functional>
#include <string>
#include <mutex>
#include "StreamCon.h"
#include "StreamSession.h"

class StreamMgr;

/// <summary>
/// Stream connection data.
/// Implements a connection pattern.
/// 
/// A token for a StreamSession. When the webcamera contents a stream
/// need to be accessed and that access has a lifetime (i.e., if we're
/// showing the camera stream to a window, it should keep streaming until
/// the window is closed). In that case, the window should have a StreamCon
/// object that gets destructed with the window's destruction.
/// 
/// Multiple StreamCons can exist for a StreamSession. When all StreamCons
/// are destroyed, the StreamSession will automatically disconnect and be
/// destroyed.
/// 
/// The StreamCon also has some interface utilities to interact with the
/// StreamSession. These utilities also respect threading needs.
/// </summary>
class StreamCon
{
public:
	/// <summary>
	/// It's assumed while there's a StreamCon, the parent
	/// StreamSession will be valid. Meaning it's the StreamSession's
	/// job to invalidate all existing children StreamCon before 
	/// destruction.
	/// </summary>
	StreamSession * parentSession;
public:

	/// <summary>
	/// If true, the StreamCon is valid (connected to a StreamSession). 
	/// Else, false. A StreamCon can become false if the SteamSession is
	/// explicitly deleted while something is still holding on to the 
	/// StreamCon token. In this case, the StreamCok isn't destroyed but
	/// is invalidated.
	/// </summary>
	bool owned;

public:
	/// <summary>
	/// Cached version of the parent session's URI.
	/// </summary>
	std::string streamURI;

	/// <summary>
	/// Callback function for when the StreamSession is disconnected. This
	/// callback is unique to the StreamCon.
	/// </summary>
	std::function<void(void)> fnOnDC;

	/// <summary>
	/// Callback function for when the StreamSession encounters an error. This
	/// callback is unique to the StreamCon. 
	/// </summary>
	/// <remarks>Unsure if this is actually used.</remarks>
	std::function<void(void)> fnOnErr;

	/// <summary>
	/// Callback function for when the StreamSession has processed a new
	/// video frame. This callback is unique to the StreamCon. 
	/// </summary>
	/// <remarks>
	/// To access the new video frame, it should be grabbed from the parentSession,
	/// but there are threading edge cases with this method (the parent can be
	/// destroyed right when we're accessing it). A better but more complex solution
	/// would involve utility functions in the StreamCon itself to ensure the StreamSession
	/// isn't destroyed the exact moment we try to pull an image from it from a StreamCon
	/// callback function.
	/// </remarks>
	std::function<void(void)> fnOnChange;

	// A mutex that the StreamSession must take before disowning 
	// the connection.
	//
	// The connection will take this to stop the reference of it
	// parent from being cleared while it is referencing it.
	std::mutex disownMutex;

	inline bool IsOrphaned() const
	{ return !this->owned; }

	inline bool IsOwned() const
	{ return this->owned; }

	/// <summary>
	/// Reconnect the StreamCon. 
	/// 
	/// </summary>
	/// <param name="force">If true, the StreamSession tied to the StreamCon will
	/// be disconnected first, before reconnecting the video stream. If false, the
	/// StreamSession will only be reconnected if it is currently disconnected.</param>
	void Reconnect(bool force);

	void Halt();

	StreamSession::ConState QueryConnectionState();

	/// <summary>
	/// Invalidate the connection.
	/// </summary>
	/// <returns></returns>
	// !TODO: This is a minomer because of bad wording. Rename to 
	// something else like Kill(), End(), Remove(), etc.
	bool Disconnect();

	/// <summary>
	/// Clears ownership data with the StreamSession.
	/// </summary>
	/// <returns>True if success.</returns>
	/// <remarks>
	/// Should probably be better encapsulated so only StreamCon and 
	/// StreamSessions can call this.
	/// </remarks>
	bool _Clear();

public:
	StreamCon(StreamSession* parent);
	~StreamCon();

public:
	typedef std::shared_ptr<StreamCon> Ptr;
};