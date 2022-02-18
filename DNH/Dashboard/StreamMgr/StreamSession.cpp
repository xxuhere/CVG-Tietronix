#include "StreamSession.h"
#include "StreamCon.h"
#include "StreamMgr.h"

#if WIN32
	#include <windows.h> // Sleep()
#else
	#include <unistd.h>
#endif

StreamSession::StreamSession(StreamMgr * parent, const std::string & uri)
{
	this->mgr		= parent;
	this->streamURI = uri;
	this->active	= true;
}

StreamSession::~StreamSession()
{
	std::lock_guard<std::mutex> guardCon(this->connectionsMut);
	if(this->active == true)
	{ 
		this->active = false;
		this->processingThread.join();
	}
}

void StreamSession::Reconnect(bool force)
{
	this->reconnectCmd = 
		force ? 
			Recon::Hard :
			Recon::Soft;
}

void StreamSession::DisconnectSession(bool unreg, bool joinThread)
{
	this->active = false;

	{ // Guard scope
		std::lock_guard<std::mutex> guard(this->connectionsMut);

		if(this->stream.isOpened())
		{
			this->width = -1;
			this->height = -1;

			for(StreamCon::Ptr c : this->connections)
			{
				std::lock_guard<std::mutex> disownGuard(c->disownMutex);
				c->_Clear();
			}

			this->stream.release();
		}
		this->connections.clear();
	}

	// Joining needs to be done before Unregister - because that may
	// cause a destructor call - which will conflict with the current
	// state the StreamSession is in.
	if(joinThread)
		this->processingThread.join();

	if(unreg)
		this->mgr->Unregister(this);
}

bool StreamSession::Disconnect(StreamCon* con)
{
	bool emptySess = false;
	bool rem = false;
	{
		std::lock_guard<std::mutex> guard(this->connectionsMut);

		for(
			auto it = this->connections.begin(); 
			it != this->connections.end(); 
			++it)
		{
			if(it->get() == con)
			{
				{
					std::lock_guard<std::mutex> disownGuard((*it)->disownMutex);
					(*it)->_Clear();
				}
				// The lock guard shouldn't be taken during erase, as erasing 
				// can trigger the smart pointer to delete.
				this->connections.erase(it);
				rem = true;

				emptySess = (this->connections.size() == 0);
				break;
			}
		}

		if(!rem)
			return false;
	}
	if(emptySess)
		this->DisconnectSession();

	return true;
}

void StreamSession::Halt()
{
	if(this->active == false)
		return;

	this->reconnectCmd = Recon::Halt;
}

void StreamSession::_ThreadFunction()
{
	this->conState = ConState::Disconnected;

	cv::VideoCapture cv;

	// Note that a stream can be inactive, but still have its
	// thread function running. This is because a reset command
	// will trigger an attempt to reconnect.

	// This loop allows us to return to connecting on a hard reset.
	while(this->active)
	{ 
		this->conState = ConState::Connecting;
		cv.open(this->streamURI, cv::CAP_FFMPEG);

		if(!cv.isOpened())
		{
			this->active = false;
			return;
		}

		this->width	= (int)cv.get(cv::CAP_PROP_FRAME_WIDTH);
		this->height= (int)cv.get(cv::CAP_PROP_FRAME_HEIGHT);

		// The loop for handling the video streaming when connected
		while(this->active && cv.isOpened())
		{
			this->conState = ConState::Connected;

			// Polling for a connection command. A hard reconnect
			// or connection halt will interrupt the current 
			// play/streaming loop.
			if(
				this->reconnectCmd == Recon::Hard || 
				this->reconnectCmd == Recon::Halt)
			{
				break;
			}
			else if(this->reconnectCmd == Recon::Soft)
				this->reconnectCmd = Recon::Void;

			std::shared_ptr<cv::Mat> sptrMat = 
				std::shared_ptr<cv::Mat>(new cv::Mat);

			cv >> *sptrMat;

			if(!sptrMat->empty())
			{
				std::lock_guard<std::mutex> guardImg(this->imageDataMut);
				this->textureDirty = true;
				this->imageData = sptrMat;
			}

			// Broadcast change. Make sure imageDataMut isn't locked at 
			// the time because the event handlers will no doubt want to
			// analyze the images data.
			{
				std::lock_guard<std::mutex> guardCons(this->connectionsMut);
				for(StreamCon::Ptr scon : this->connections)
				{
					if(scon->fnOnChange)
						scon->fnOnChange();
				}
			}

			// Note that the Sleep amount should also respect the 
			// amount of time it took to do the above.
#if WIN32
			Sleep(30);
#else
			usleep(30);
#endif
		}

		this->conState = ConState::Disconnected;

		// If not connected, keep the connection open but poll to
		// see if a reset command has occured to attempt to reconnect.
		while(this->active)
		{
			// If disconnected and there's no reconnect command,
			// stay in a disconnected holding pattern.
			if(
				this->reconnectCmd != Recon::Void && 
				this->reconnectCmd != Recon::Halt)
			{
				this->reconnectCmd = Recon::Void;
				break;
			}
#if WIN32
			Sleep(30);
#else
			usleep(30);
#endif
		}
	}

	this->conState = ConState::Unknown;
}

void StreamSession::_StartThread()
{
	this->processingThread = 
		std::thread(
			[this]()
			{
				this->_ThreadFunction();

				// This should already be set before exiting 
				// ThreadFunction(), but just in case.
				this->active = false;
			});
}

std::shared_ptr<StreamCon> StreamSession::MakeConnection()
{
	StreamCon::Ptr ret = StreamCon::Ptr(new StreamCon(this));
	{
		std::lock_guard<std::mutex> guard(this->connectionsMut);
		this->connections.push_back(ret);
	}
	return ret;
}