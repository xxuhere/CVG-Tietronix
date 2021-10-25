#pragma once
#include <memory>
#include "server_ws.hpp"
#include "json.hpp"

using WsServer = SimpleWeb::SocketServer<SimpleWeb::WS>;
typedef std::shared_ptr<WsServer::Connection> WSConSPtr;