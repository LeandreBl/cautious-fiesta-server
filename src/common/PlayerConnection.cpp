#include <iostream>

#include "PlayerConnection.hpp"
#include "GameRoom.hpp"
#include "Server.hpp"

namespace cf
{
PlayerConnection::PlayerConnection(std::unique_ptr<tcp::socket> &socket,
				   Server &server) noexcept
    : _service(), _header(), _rd(0), _buffer(), _toRead(0), _payload(),
      _name(""), _udpSocket(), _udpIndex(0), _udpRemote(),
      _tcpSocket(std::move(socket)), _room(nullptr), _toWrite(),
      _server(server), _player(), _ready(false)
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
	boost::system::error_code err;
	size_t n = _tcpSocket->write_some(
		boost::asio::buffer(serializer.getNativeHandle(),
				    serializer.getSize()),
		err);
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
	return (int)_tcpSocket->lowest_layer().native_handle();
}

void PlayerConnection::close() noexcept
{
	try {
		_tcpSocket->shutdown(tcp::socket::shutdown_receive);
		_tcpSocket->close();
	} catch (const boost::system::system_error &error) {
	}
	_name.clear();
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

void PlayerConnection::pushPacket(Serializer &packet,
				  enum pktType_e type) noexcept
{
	TcpPacketHeader header(type, packet.getSize());
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

void PlayerConnection::setPlayer(const Player::stats &stats) noexcept
{
	_player = Player(_name, stats);
}

Player &PlayerConnection::getPlayer() noexcept
{
	return _player;
}

void PlayerConnection::setUdpPort(uint16_t port) noexcept
{
	_udpSocket = std::make_unique<udp::socket>(_service);
	_udpSocket->open(udp::v4());
	_udpRemote =
		udp::endpoint(_tcpSocket->local_endpoint().address(), port);
}

void PlayerConnection::pushUdp(Serializer &packet,
			       enum UdpPrctl::type type) noexcept
{
	_toWriteUdp.emplace(packet, type, _udpIndex++);
}

void PlayerConnection::refreshUdp() noexcept
{
	while (_toWriteUdp.size()) {
		auto &pkt = _toWriteUdp.front();
		if (writeUdp(pkt) != 0)
			return;
		_toWriteUdp.pop();
	}
}

int PlayerConnection::writeUdp(const Serializer &serializer) noexcept
{
	boost::system::error_code err;
	_udpSocket->send_to(boost::asio::buffer(serializer.getNativeHandle(),
						serializer.getSize()),
			    _udpRemote, MSG_CONFIRM, err);
	if (err) {
		std::cerr << err.message() << std::endl;
		return -1;
	}
	return 0;
}

} // namespace cf