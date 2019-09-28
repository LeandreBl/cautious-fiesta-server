#pragma once

#include <list>
#include <string>
#include <functional>
#include <boost/asio.hpp>
#include <Scene.hpp>

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
	void kick(PlayerConnection &handle) noexcept;
	int packetHandler(PlayerConnection &handle,
			  const TcpPacketHeader &packetHeader,
			  Serializer &payload) noexcept;

      protected:
	void handleNewConnection();
	int loginHandler(PlayerConnection &handle, Serializer &toRead);
	int logoutHandler(PlayerConnection &handle, Serializer &toRead);
	int createGameRoomHandler(PlayerConnection &handle, Serializer &toRead);
	int deleteGameRoomHandler(PlayerConnection &handle, Serializer &toRead);
	int getGameRoomsHandler(PlayerConnection &handle, Serializer &toRead);
	int joinGameRoomHandler(PlayerConnection &handle, Serializer &toRead);
	int leaveGameRoomHandler(PlayerConnection &handle, Serializer &toRead);
	int getGameRoomPlayersListHandler(PlayerConnection &handle,
					  Serializer &toRead);
	int sendGameRoomMessageHandler(PlayerConnection &handle,
				       Serializer &toRead);
	int receiveGameRoomMessageHandler(PlayerConnection &handle,
					  Serializer &toRead);
	int toggleReadyHandler(PlayerConnection &handle, Serializer &toRead);
	int requireAssetHandler(PlayerConnection &handle, Serializer &toRead);
	int sendAssetHandler(PlayerConnection &handle, Serializer &toRead);
	int gameStartHandler(PlayerConnection &handle, Serializer &toRead);
	int ackHandler(PlayerConnection &handle, Serializer &toRead);

	void resendGameRoomsHandler() noexcept;
	void resendPlayerListHandler() noexcept;

	void fillGameRooms(Serializer &packet) const noexcept;
	void fillGameRoomPlayers(const std::string &roomName,
				 Serializer &packet) const noexcept;
	template <typename... Args> void autoBind(size_t index, Args... args)
	{
		_callbacks[index] =
			std::bind(args..., this, std::placeholders::_1,
				  std::placeholders::_2);
	}

	template <typename... Args>
	void say(bool isOk, const char *format, Args... args) const noexcept
	{
		trace(isOk, "%sServer%s: ", GREEN, RESET);
		trace(format, args...);
	}
	std::function<int(PlayerConnection &handle, Serializer &toRead)>
		_callbacks[cf::ACK + 1];
	uint16_t _tcpPort;
	uint16_t _udpPort;
	boost::asio::io_service _service;
	tcp::acceptor _listener;
	std::unique_ptr<tcp::socket> _pending;
	std::list<PlayerConnection> _connectionPool;
	std::list<GameRoom> _gameRooms;
	bool _running;
};
} // namespace cf