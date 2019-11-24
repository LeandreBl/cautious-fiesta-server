#include <iostream>
#include <boost/bind.hpp>
#include <memory>
#include <filesystem>

#include <Asset.hpp>

#include "Server.hpp"

namespace cf {
Server::Server(uint16_t tcpPort, uint16_t udpPort) noexcept
	: _tcpPort(tcpPort)
	, _udpPort(udpPort)
	, _service()
	, _listener(_service)
	, _pending()
	, _connectionPool()
	, _assetsHandlers()
	, _running(false)
{
	try {
		_listener = std::move(tcp::acceptor(_service, tcp::endpoint(tcp::v4(), _tcpPort)));
	}
	catch (const boost::system::system_error &err) {
		say(false, "Error: Unable to run on port %u\n", _tcpPort);
		return;
	}
	autoBind(TcpPrctl::Type::LOGIN, &Server::loginHandler);
	autoBind(TcpPrctl::Type::LOGOUT, &Server::logoutHandler);
	autoBind(TcpPrctl::Type::CREATE_GAMEROOM, &Server::createGameRoomHandler);
	autoBind(TcpPrctl::Type::DELETE_GAMEROOM, &Server::deleteGameRoomHandler);
	autoBind(TcpPrctl::Type::GET_GAMEROOMS_LIST, &Server::getGameRoomsHandler);
	autoBind(TcpPrctl::Type::JOIN_GAMEROOM, &Server::joinGameRoomHandler);
	autoBind(TcpPrctl::Type::LEAVE_GAMEROOM, &Server::leaveGameRoomHandler);
	autoBind(TcpPrctl::Type::GET_GAMEROOM_PLAYERS_LIST, &Server::getGameRoomPlayersListHandler);
	autoBind(TcpPrctl::Type::SEND_MESSAGE, &Server::sendGameRoomMessageHandler);
	autoBind(TcpPrctl::Type::RECEIVE_MESSAGE, &Server::receiveGameRoomMessageHandler);
	autoBind(TcpPrctl::Type::TOGGLE_READY, &Server::toggleReadyHandler);
	autoBind(TcpPrctl::Type::GAME_STARTED, &Server::gameStartHandler);
	autoBind(TcpPrctl::Type::ASSETS_REQUIREMENT, &Server::requireAssetHandler);
	autoBind(TcpPrctl::Type::ASSETS_SEND, &Server::sendAssetHandler);
	autoBind(TcpPrctl::Type::ACK, &Server::ackHandler);
	say(true, "Running on port %u\n", _tcpPort);
	_running = true;
}

void Server::run() noexcept
{
	if (_running == false)
		return;
	_pending = std::make_unique<tcp::socket>(_service);
	_listener.async_accept(*_pending, boost::bind(&Server::handleNewConnection, this));
	_service.run();
}

void Server::refreshTcpConnections() noexcept
{
	for (auto &&i : _connectionPool)
		i.refreshTcp();
}

void Server::handleNewConnection()
{
	say(true, "%s: connected\n", _pending->local_endpoint().address().to_string().c_str());
	_connectionPool.emplace_back(_pending, *this);
	_pending = std::make_unique<tcp::socket>(_service);
	_listener.async_accept(*_pending, boost::bind(&Server::handleNewConnection, this));
}

void Server::kick(PlayerConnection &handle) noexcept
{
	if (handle.isInRoom() && handle.room().isRunning()) {
		handle.name("");
		for (auto &&i : handle.room().getPlayers()) {
			if (i->isLogged() == true)
				return;
		}
		handle.room().stop();
		return;
	}
	for (auto it = _connectionPool.begin(); it != _connectionPool.end(); ++it) {
		if (handle.getId() == it->getId()) {
			it->leaveRoom();
			say(true, "%s: disconnected\n", it->getIp().c_str());
			it->close();
			_connectionPool.erase(it);
			return;
		}
	}
}

void Server::kickFromRoom(GameRoom &room) noexcept
{
	for (auto &&i : room.getPlayers()) {
		kick(*i);
	}
}

void Server::stop() noexcept
{
	_service.stop();
	_running = false;
}

boost::asio::io_service &Server::getService() noexcept
{
	return _service;
}

AssetHandler::AssetHandler(Server &server, const std::string &filename) noexcept
	: port(0)
	, filesize(0)
	, filename(filename)
	, chksum(0)
	, receiver(server.getService())
	, acceptor(server.getService(), tcp::endpoint(tcp::v4(), 0))
	, server(server)
{
	this->filesize = std::filesystem::file_size(this->filename);
	this->port = this->acceptor.local_endpoint().port();
	this->chksum = common::computeChksum(this->filename);
}
} // namespace cf