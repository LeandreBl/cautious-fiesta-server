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
#include "Trace.hpp"

namespace cf
{
class Server;
struct AssetHandler {
	AssetHandler(Server &server, const std::string &filename) noexcept;
	uint16_t port;
	uint64_t filesize;
	std::string filename;
	uint64_t chksum;
	tcp::socket receiver;
	tcp::acceptor acceptor;
	Server &server;
	std::ifstream file;
	char buffer[4096];
};
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
	void refreshTcpConnections() noexcept;
	boost::asio::io_service &getService() noexcept;
	uint64_t easyChksum(const std::string &filename) noexcept;

      protected:
	void kickRoomPlayers(PlayerConnection &handle) noexcept;
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
	void sendRequiredAssets(PlayerConnection &handle) noexcept;
	void startGameRoom(const GameRoom &room) noexcept;

	int deleteGameRoom(const std::string &name) noexcept;

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

	void assetListenerCallback(AssetHandler &handler,
				   const boost::system::error_code &err);
	void assetWriterCallback(AssetHandler &handler,
				 const boost::system::error_code &err);

	void gameRoomTermination(GameRoom &room);

	std::function<int(PlayerConnection &handle, Serializer &toRead)>
		_callbacks[cf::ACK + 1];
	uint16_t _tcpPort;
	uint16_t _udpPort;
	boost::asio::io_service _service;
	tcp::acceptor _listener;
	std::unique_ptr<tcp::socket> _pending;
	std::list<PlayerConnection> _connectionPool;
	std::list<GameRoom> _gameRooms;
	std::list<GameRoom> _runningGameRooms;
	std::list<AssetHandler> _assetsHandlers;
	bool _running;
};
} // namespace cf