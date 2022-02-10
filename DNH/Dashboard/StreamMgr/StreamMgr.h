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
	static StreamMgr & Instance();

private:
	StreamMgr();
	~StreamMgr();

	std::mutex streamsMut;
	std::map<std::string, StreamSession::Ptr> streams;

public:
	StreamCon::Ptr ConnectToURL(const std::string& uri);

	void Clear(bool joinThreads);

	bool Unregister(StreamSession* ss);

	static std::string CanonicalizeURI(const std::string& str);
};