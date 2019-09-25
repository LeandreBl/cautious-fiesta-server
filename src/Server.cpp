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
		_listener.bind(tcp::endpoint(tcp::v4(), _tcpPort));
	} catch (const boost::system::system_error &err) {
		say(false, "Error: Unable to run on port %u\n", _tcpPort);
		return;
	}
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
	say(true, "%s: connected",
	    _pending->local_endpoint().address().to_string().c_str());
	_connectionPool.emplace_back(_pending);
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