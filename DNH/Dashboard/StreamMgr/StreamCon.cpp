#include "StreamCon.h"
#include "StreamSession.h"

StreamCon::StreamCon(StreamSession* parent)
{
	this->parentSession = parent;
	this->owned			= this;
	this->streamURI		= parent->URI();
}

StreamCon::~StreamCon()
{
	this->Disconnect();
}

void StreamCon::Reconnect(bool force)
{
	std::lock_guard<std::mutex> disownGuard(this->disownMutex);

	StreamSession * strsess = this->parentSession;
	if( strsess == nullptr)
		return;

	strsess->Reconnect(force);
}

void StreamCon::Halt()
{
	std::lock_guard<std::mutex> disownGuard(this->disownMutex);

	StreamSession * strsess = this->parentSession;
	if( strsess == nullptr)
		return;

	strsess->Halt();
}

StreamSession::ConState StreamCon::QueryConnectionState()
{
	std::lock_guard<std::mutex> disownGuard(this->disownMutex);

	StreamSession * strsess = this->parentSession;
	if( strsess == nullptr)
		return StreamSession::ConState::Unknown;

	return strsess->ConnectionState();
}

bool StreamCon::Disconnect()
{
	if(this->parentSession)
		return this->parentSession->Disconnect(this);

	return false;
}

bool StreamCon::_Clear()
{
	this->parentSession = nullptr;
	this->fnOnDC		= nullptr;
	this->fnOnErr		= nullptr;
	this->fnOnChange	= nullptr;
	this->owned			= false;
	this->streamURI.clear();

	return true;
}