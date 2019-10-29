#include <iostream>
#include <boost/bind.hpp>
#include <memory>
#include <filesystem>

#include "Server.hpp"

namespace cf
{
Server::Server(uint16_t tcpPort, uint16_t udpPort) noexcept
    : _tcpPort(tcpPort), _udpPort(udpPort), _service(), _listener(_service),
      _pending(), _connectionPool(), _assetsHandlers(), _running(false)
{
	try {
		_listener = std::move(tcp::acceptor(
			_service, tcp::endpoint(tcp::v4(), _tcpPort)));
	} catch (const boost::system::system_error &err) {
		say(false, "Error: Unable to run on port %u\n", _tcpPort);
		return;
	}
	autoBind(cf::LOGIN, &Server::loginHandler);
	autoBind(cf::LOGOUT, &Server::logoutHandler);
	autoBind(cf::CREATE_GAMEROOM, &Server::createGameRoomHandler);
	autoBind(cf::DELETE_GAMEROOM, &Server::deleteGameRoomHandler);
	autoBind(cf::GET_GAMEROOMS_LIST, &Server::getGameRoomsHandler);
	autoBind(cf::JOIN_GAMEROOM, &Server::joinGameRoomHandler);
	autoBind(cf::LEAVE_GAMEROOM, &Server::leaveGameRoomHandler);
	autoBind(cf::GET_GAMEROOM_PLAYERS_LIST,
		 &Server::getGameRoomPlayersListHandler);
	autoBind(cf::SEND_MESSAGE, &Server::sendGameRoomMessageHandler);
	autoBind(cf::RECEIVE_MESSAGE, &Server::receiveGameRoomMessageHandler);
	autoBind(cf::TOGGLE_READY, &Server::toggleReadyHandler);
	autoBind(cf::GAME_STARTED, &Server::gameStartHandler);
	autoBind(cf::ASSETS_REQUIREMENT, &Server::requireAssetHandler);
	autoBind(cf::ASSETS_SEND, &Server::sendAssetHandler);
	autoBind(cf::ACK, &Server::ackHandler);
	say(true, "Running on port %u\n", _tcpPort);
	_running = true;
}

void Server::run() noexcept
{
	if (_running == false)
		return;
	_pending = std::make_unique<tcp::socket>(_service);
	_listener.async_accept(*_pending,
			       boost::bind(&Server::handleNewConnection, this));
	_service.run();
}

void Server::refreshTcpConnections() noexcept
{
	for (auto &&i : _connectionPool)
		i.refreshTcp();
}

void Server::handleNewConnection()
{
	say(true, "%s: connected\n",
	    _pending->local_endpoint().address().to_string().c_str());
	_connectionPool.emplace_back(_pending, *this);
	_pending = std::make_unique<tcp::socket>(_service);
	_listener.async_accept(*_pending,
			       boost::bind(&Server::handleNewConnection, this));
}

void Server::kick(PlayerConnection &handle) noexcept
{
	for (auto it = _connectionPool.begin(); it != _connectionPool.end();
	     ++it) {
		if (handle.getId() == it->getId()) {
			say(true, "%s: disconnected\n", it->getIp().c_str());
			it->close();
			it->leaveRoom();
			_connectionPool.erase(it);
			return;
		}
	}
}

void Server::stop() noexcept
{
	_service.stop();
	_running = false;
}

uint64_t Server::easyChksum(const std::string &filename) noexcept
{
	std::ifstream file;
	char buffer[4096];
	uint64_t chk = 0;

	file.open(filename);
	if (!file.is_open())
		return 0;
	std::streamsize rd;
	do {
		rd = file.readsome(buffer, sizeof(buffer));
		for (std::streamsize i = 0; i < rd; ++i)
			chk += ~buffer[i] & 0xF0F0F0F0F;
	} while (rd == sizeof(buffer));
	return chk;
}

boost::asio::io_service &Server::getService() noexcept
{
	return _service;
}

AssetHandler::AssetHandler(Server &server, const std::string &filename) noexcept
    : port(0), filesize(0), filename(filename), chksum(0),
      receiver(server.getService()),
      acceptor(server.getService(), tcp::endpoint(tcp::v4(), 0)), server(server)
{
	this->filesize = std::filesystem::file_size(this->filename);
	this->port = this->acceptor.local_endpoint().port();
	this->chksum = server.easyChksum(this->filename);
}

AssetHandler &AssetHandler::operator=(AssetHandler &v)
{
	return v;
}

void AssetHandler::operator=(AssetHandler &&v)
{
	*this = std::move(v);
}

} // namespace cf