#include <iostream>
#include <boost/bind.hpp>
#include <memory>

#include "Server.hpp"

namespace cf
{
Server::Server(uint16_t tcpPort, uint16_t udpPort) noexcept
    : _tcpPort(tcpPort), _udpPort(udpPort), _service(), _listener(_service),
      _pending(), _connectionPool(), _running(false)
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
	autoBind(cf::GET_GAMEROOMS_LIST,
		 &Server::getGameRoomsHandler);
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

void Server::handleNewConnection()
{
	say(true, "%s: connected\n",
	    _pending->local_endpoint().address().to_string().c_str());
	_connectionPool.emplace_back(_pending, *this);
	_pending = std::make_unique<tcp::socket>(_service);
	_listener.async_accept(*_pending,
			       boost::bind(&Server::handleNewConnection, this));
}

void Server::stop() noexcept
{
	_service.stop();
	_running = false;
}
} // namespace cf