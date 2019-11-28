#include <iostream>

#include <boost/bind.hpp>

#include "PlayerConnection.hpp"
#include "GameRoom.hpp"
#include "Server.hpp"

namespace cf {
static int id = 0;
PlayerConnection::PlayerConnection(std::unique_ptr<tcp::socket> &socket, Server &server) noexcept
	: _id(id++)
	, _header()
	, _rd(0)
	, _netBuffer()
	, _toRead(0)
	, _name("")
	, _ingameSocket()
	, _tcpSocket(std::move(socket))
	, _room(nullptr)
	, _toWrite()
	, _toWriteUdp()
	, _udpIndex(0)
	, _server(server)
	, _player()
	, _ready(false)
{
	headerMode();
}

void PlayerConnection::headerMode() noexcept
{
	_tcpSocket->async_read_some(boost::asio::buffer(&_header + _rd, sizeof(_header) - _rd),
				    boost::bind(&PlayerConnection::asyncReadHeader, this,
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
}

void PlayerConnection::packetMode() noexcept
{
	size_t n = (_toRead > _netBuffer.buffer.size()) ? _netBuffer.buffer.size() : _toRead;
	_tcpSocket->async_read_some(boost::asio::buffer(_netBuffer.buffer, n),
				    boost::bind(&PlayerConnection::asyncReadPayload, this,
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
}

void PlayerConnection::asyncReadPayload(const boost::system::error_code &error,
					std::size_t bytes_transferred)
{
	if (error == boost::asio::error::eof) {
		_server.kick(*this);
		return;
	}
	else if (error) {
		std::cerr << "~" << _name << " Async read error: " << error.message() << std::endl;
		_server.kick(*this);
		return;
	}
	_toRead -= bytes_transferred;
	_netBuffer.serializer.nativeSet(_netBuffer.buffer.data(), bytes_transferred);
	if (_toRead == 0) {
		_server.packetHandler(*this, _header, _netBuffer.serializer);
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
	}
	else if (error) {
		std::cerr << "~" << _name << " Async read error: " << error.message() << std::endl;
		_server.kick(*this);
		return;
	}
	_rd += bytes_transferred;
	if (_rd == sizeof(_header)) {
		_rd = 0;
		_toRead = _header.getLength();
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
	boost::system::error_code err;
	size_t n = _tcpSocket->write_some(
		boost::asio::buffer(serializer.getNativeHandle(), serializer.getSize()), err);
	if (err)
		return -1;
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
	return _id;
}

void PlayerConnection::close() noexcept
{
	try {
		_tcpSocket->shutdown(tcp::socket::shutdown_both);
		_tcpSocket->close();
	}
	catch (const boost::system::system_error &error) {
		std::cerr << error.what() << std::endl;
	}
	_name.clear();
}

void PlayerConnection::closeUdp() noexcept
{
	try {
		_ingameSocket->shutdown(tcp::socket::shutdown_both);
		_ingameSocket->close();
	}
	catch (const boost::system::system_error &error) {
		std::cerr << error.what() << std::endl;
	}
	while (!_toWriteUdp.empty())
		_toWriteUdp.pop();
	_udpBuffer.serializer.clear();
}

void PlayerConnection::assetReady(bool state) noexcept
{
	_assetReady = state;
}

bool PlayerConnection::ready() const noexcept
{
	return _ready && _assetReady;
}

void PlayerConnection::ready(bool state) noexcept
{
	_ready = state;
}

void PlayerConnection::pushPacket(Serializer &packet, TcpPrctl::Type type) noexcept
{
	TcpPrctl header(type, packet.getSize());
	header.display(false);
	_toWrite.emplace(packet, type);
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

std::string PlayerConnection::getIp() const noexcept
{
	return _tcpSocket->local_endpoint().address().to_string();
}

void PlayerConnection::setPlayer(const Stats::stats &stats, const sf::Color &color) noexcept
{
	_player = Stats(_name, stats, color);
}

Stats &PlayerConnection::getPlayer() noexcept
{
	return _player;
}

struct netBuffer &PlayerConnection::getUdpBuffer() noexcept
{
	return _udpBuffer;
}

tcp::socket &PlayerConnection::setSocket(boost::asio::io_service &service) noexcept
{
	_ingameSocket = std::make_unique<tcp::socket>(service);
	return *_ingameSocket;
}

void PlayerConnection::pushUdpPacket(const Serializer &packet, UdpPrctl::Type type) noexcept
{
	_toWriteUdp.emplace(packet, type, _udpIndex++);
}

tcp::socket &PlayerConnection::getIngameSocket() noexcept
{
	return *_ingameSocket;
}

void PlayerConnection::refreshUdp() noexcept
{
	while (!_toWriteUdp.empty()) {
		auto &p = _toWriteUdp.front();
		int flg = 0;
		boost::system::error_code e;
		_ingameSocket->send(boost::asio::buffer(p.getNativeHandle(), p.getSize()), flg, e);
		if (e) {
			trace(false, "~%s: %s\n", _name.c_str(), e.message().c_str());
			return;
		}
		_toWriteUdp.pop();
	}
}
} // namespace cf
