#pragma once

#include <list>
#include <string>
#include <Scene.hpp>
#include <boost/asio.hpp>

#include "Serializer.hpp"
#include "PlayerConnection.hpp"
#include "GameRoom.hpp"
#include "colors.h"
#include "trace.hpp"

namespace cf
{
class Server
{
      public:
	Server(uint16_t tcpPort = 2121, uint16_t udpPort = 2122) noexcept;
	~Server() = default;
	void run() noexcept;
	void stop() noexcept;

      protected:
	void handleNewConnection();
	template <typename... Args>
	void say(bool isOk, const char *format, Args... args) const noexcept
	{
		trace(isOk, "%sServer%s: ", GREEN, RESET);
		trace(format, args...);
	}
	uint16_t _tcpPort;
	uint16_t _udpPort;
	boost::asio::io_service _service;
	tcp::acceptor _listener;
	std::unique_ptr<tcp::socket> _pending;
	std::list<PlayerConnection> _connectionPool;
	bool _running;
	// rooms
	// listeners
	// players
};
} // namespace cf