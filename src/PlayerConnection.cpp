#include <iostream>

#include "PlayerConnection.hpp"
#include "GameRoom.hpp"
#include "Server.hpp"

namespace cf
{
PlayerConnection::PlayerConnection(std::unique_ptr<tcp::socket> &socket,
				   Server &server) noexcept
    : _header(), _rd(0), _buffer(), _toRead(0), _payload(), _name(""),
      _udpSocket(), _tcpSocket(std::move(socket)), _room(nullptr), _toWrite(),
      _server(server), _ready(false)
{
	headerMode();
}

void PlayerConnection::headerMode() noexcept
{
	_tcpSocket->async_read_some(
		boost::asio::buffer(&_header + _rd, sizeof(_header) - _rd),
		std::bind(&PlayerConnection::asyncReadHeader, this,
			  std::placeholders::_1, std::placeholders::_2));
}

void PlayerConnection::packetMode() noexcept
{
	size_t n = (_toRead > _buffer.size()) ? _buffer.size() : _toRead;
	_tcpSocket->async_read_some(
		boost::asio::buffer(_buffer, n),
		std::bind(&PlayerConnection::asyncReadPayload, this,
			  std::placeholders::_1, std::placeholders::_2));
}

void PlayerConnection::asyncReadPayload(const boost::system::error_code &error,
					std::size_t bytes_transferred)
{
	if (error == boost::asio::error::eof) {
		_server.kick(*this);
		return;
	} else if (error) {
		std::cerr << "~" << _name
			  << " Async read error: " << error.message()
			  << std::endl;
		_server.kick(*this);
		return;
	}
	_toRead -= bytes_transferred;
	_payload.nativeSet(_buffer.data(), bytes_transferred);
	if (_toRead == 0) {
		_server.packetHandler(*this, _header, _payload);
		headerMode();
		return;
	}
	packetMode();
}

void PlayerConnection::asyncReadHeader(const boost::system::error_code &error,
				       std::size_t bytes_transferred)
{
	if (error == boost::asio::error::eof) {
		_server.kick(*this);
		return;
	} else if (error) {
		std::cerr << "~" << _name
			  << " Async read error: " << error.message()
			  << std::endl;
		_server.kick(*this);
		return;
	}
	_rd += bytes_transferred;
	if (_rd == sizeof(_header)) {
		_rd = 0;
		_toRead = _header.getLen();
		packetMode();
		return;
	}
	headerMode();
}

void PlayerConnection::name(const std::string &name) noexcept
{
	_name = name;
}

const std::string &PlayerConnection::name() const noexcept
{
	return _name;
}

bool PlayerConnection::isLogged() const noexcept
{
	return !_name.empty();
}

int PlayerConnection::writeTcp(const Serializer &serializer) noexcept
{
	size_t n = _tcpSocket->write_some(boost::asio::buffer(
		serializer.getNativeHandle(), serializer.getSize()));
	if (n != serializer.getSize()) {
		_server.kick(*this);
		return -1;
	}
	return 0;
}

bool PlayerConnection::isInRoom() const noexcept
{
	return _room != nullptr;
}

GameRoom &PlayerConnection::room() const noexcept
{
	return *_room;
}

void PlayerConnection::room(GameRoom &room) noexcept
{
	_room = &room;
}

int PlayerConnection::getId() const noexcept
{
	return (int)_tcpSocket->lowest_layer().native_handle();
}

void PlayerConnection::close() noexcept
{
	try {
		_tcpSocket->shutdown(tcp::socket::shutdown_receive);
		_tcpSocket->close();
	} catch (const boost::system::error_code &error) {
		std::cerr << error.message() << std::endl;
	}
	_name.clear();
}

bool PlayerConnection::ready() const noexcept
{
	return _ready;
}

void PlayerConnection::ready(bool state) noexcept
{
	_ready = state;
}

void PlayerConnection::pushPacket(Serializer &packet,
				  enum pktType_e type) noexcept
{
	packet.setHeader(type);
	_toWrite.emplace(packet);
}


void PlayerConnection::refreshTcp() noexcept
{
	while (_toWrite.size()) {
		auto &pkt = _toWrite.front();
		writeTcp(pkt);
		_toWrite.pop();
	}
}

void PlayerConnection::leaveRoom() noexcept
{
	if (_room) {
		_room->leave(*this);
		_room = nullptr;
	}
}

} // namespace cf
