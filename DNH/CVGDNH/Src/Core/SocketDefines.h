#pragma once
#include "CompatabilityHacks.h" // Suppress 4996

#include <memory>
#include "Simple-WebSocket-Server/server_ws.hpp"
#include "nlohmann/json.hpp"



using WsServer = SimpleWeb::SocketServer<SimpleWeb::WS>;
typedef std::shared_ptr<WsServer::Connection> WSConSPtr;