#pragma once

#include "StreamCon.h"
#include "StreamSession.h"

#include <map>
#include <mutex>
#include <string>

/// <summary>
/// A manager for all OpenCV RTSP streams. It makes sure everything
/// referencing the same URI (i.e., StreamSession) only create a single
/// stream, and reuse it across the application instead of creating
/// multiple networked video streams - which will tax both the client
/// and server more.
/// </summary>
class StreamMgr
{
public:
	/// <summary>
	/// Get the singleton instance of the manager.
	/// </summary>
	static StreamMgr & Instance();

private:
	StreamMgr();
	~StreamMgr();

	/// <summary>
	/// The mutex to guard threaded read/write to this->streams.
	/// </summary>
	std::mutex streamsMut;

	/// <summary>
	/// The StreamSessions current active. 
	/// The key is a (canonical) URI to the stream session.
	/// The value is StreamSession mapped to the URI.
	/// </summary>
	std::map<std::string, StreamSession::Ptr> streams;

public:
	/// <summary>
	/// Get a new StreamCon to a URI.
	/// </summary>
	/// <param name="uri">The URI to generate a StreamCon for.</param>
	/// <returns>The created StreamCon.</returns>
	StreamCon::Ptr ConnectToURL(const std::string& uri);

	/// <summary>
	/// Clear all StreamSessions.
	/// </summary>
	/// <param name="joinThreads">
	/// If true, block the invoking thread and wait for all
	/// closing StreamSession threads to join.
	/// </param>
	void Clear(bool joinThreads);

	/// <summary>
	/// Remove a StreamSession.
	/// </summary>
	/// <param name="ss">
	/// The StreamSession to remove.
	/// </param>
	/// <remarks>
	/// This will invalidate all StreamCon objects that were tied 
	/// to the StreamSession.
	/// </remarks>
	/// <returns>True if success.</returns>
	bool Unregister(StreamSession* ss);

	/// <summary>
	/// Convert a URI string to a video stream to a canonical form.
	/// 
	/// The whole point of the StreamMgr is to not create excess network
	/// streaming sessions if multiple things use the same video stream,
	/// because that single video stream can be reused instead of creating
	/// duplicate network connections. This can be thwarted if multiple
	/// URIs aren't literally the same strings, but can point to the same 
	/// end-server.
	/// </summary>
	/// <param name="str">The URI to canonicalize.</param>
	/// <returns>The canonical form.</returns>
	/// <remarks>https://en.wikipedia.org/wiki/Canonicalization</remarks>
	static std::string CanonicalizeURI(const std::string& str);
};