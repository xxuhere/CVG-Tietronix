#pragma once

#include <memory>
#include <functional>
#include <string>
#include <mutex>
#include "StreamCon.h"

class StreamMgr;
class StreamSession;

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
	bool owned;

public:
	/// <summary>
	/// Cached version of the parent session's URI.
	/// </summary>
	std::string streamURI;

	std::function<void(void)> fnOnDC;
	std::function<void(void)> fnOnErr;
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

	void Reconnect(bool force);

	bool Disconnect();

	bool _Clear();

public:
	StreamCon(StreamSession* parent);
	~StreamCon();

public:
	typedef std::shared_ptr<StreamCon> Ptr;
};