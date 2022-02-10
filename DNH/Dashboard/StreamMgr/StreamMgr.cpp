#include "StreamMgr.h"


StreamMgr::StreamMgr()
{
}

StreamMgr::~StreamMgr()
{
}

StreamMgr & StreamMgr::Instance()
{
	static StreamMgr inst;
	return inst;
}

StreamCon::Ptr StreamMgr::ConnectToURL(const std::string& uri)
{
	if(uri.empty())
		return nullptr;

	std::lock_guard<std::mutex> guard(this->streamsMut);

	std::string struse = CanonicalizeURI(uri);
	auto it = this->streams.find(struse);

	StreamSession::Ptr sess;
	if(it == this->streams.end())
	{
		sess = 
			std::shared_ptr<StreamSession>(
				new StreamSession(this, struse));

		this->streams[struse] = sess;
		sess->_StartThread();
	}
	else
		sess = it->second;

	StreamCon::Ptr ret = sess->MakeConnection();
	return ret;
}

void StreamMgr::Clear(bool joinThreads)
{
	std::lock_guard<std::mutex> guard(this->streamsMut);
	for(auto it : this->streams)
	{
		it.second->DisconnectSession(false, joinThreads);
	}
	this->streams.clear();
}

bool StreamMgr::Unregister(StreamSession * ss)
{
	std::lock_guard<std::mutex> guard(this->streamsMut);

	for(
		auto it = this->streams.begin(); 
		it != this->streams.end(); 
		++it)
	{
		if(it->second.get() == ss)
		{
			this->streams.erase(it);
			return true;
		}
	}
	return false;
}

std::string StreamMgr::CanonicalizeURI(const std::string& str)
{
	// TODO:
	// Currently an empty implementation. This should take any type of string
	// and reduce it to its "most agreeable" form, so duplicate parameter strings 
	// that arent not literally the same but still point to the same place should
	// return the same output.
	return str;
}