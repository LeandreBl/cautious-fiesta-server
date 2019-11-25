#include <iostream>

#include <boost/bind.hpp>

#include "PlayerConnection.hpp"
#include "GameRoom.hpp"
#include "Server.hpp"

namespace cf {
PlayerConnection::PlayerConnection(std::unique_ptr<tcp::socket> &socket, Server &server) noexcept
	: _service()
	, _header()
	, _rd(0)
	, _netBuffer()
	, _toRead(0)
	, _name("")
	, _udpRemote(udp::endpoint(socket->remote_endpoint().address(),
				   socket->remote_endpoint().port()))
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
	return (int)_tcpSocket->lowest_layer().native_handle();
}

void PlayerConnection::close() noexcept
{
	try {
		_tcpSocket->shutdown(tcp::socket::shutdown_receive);
		_tcpSocket->close();
	}
	catch (const boost::system::system_error &error) {
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

udp::endpoint &PlayerConnection::getUdpRemote() noexcept
{
	return _udpRemote;
}

void PlayerConnection::pushUdpPacket(Serializer &packet, UdpPrctl::Type type) noexcept
{
	_toWriteUdp.emplace_back(UdpPrctl(type, packet.getSize(), _udpIndex++), packet);
	if (_udpIndex >= UINT16_MAX - 10)
		_udpIndex = 0;
}

void PlayerConnection::pushUdpAck(const UdpPrctl &header) noexcept
{
	_ackQueue.emplace(UdpPrctl::Type::ACK, 0, header.getIndex());
}

void PlayerConnection::refreshUdp(udp::socket &socket) noexcept
{
	while (!_ackQueue.empty()) {
		auto &p = _ackQueue.front();
		socket.send_to(boost::asio::buffer(&p.getNativeHandle(), sizeof(p)), _udpRemote);
		_ackQueue.pop();
	}
	for (auto &&i : _toWriteUdp) {
		std::array<boost::asio::const_buffer, 2> v;
		v[0] = boost::asio::buffer(&i.first.getNativeHandle(),
					   sizeof(i.first.getNativeHandle()));
		v[1] = boost::asio::buffer(i.second.getNativeHandle(), i.second.getSize());
		socket.send_to(v, _udpRemote);
	}
}

void PlayerConnection::notifyUdpReceive(uint16_t pktIndex) noexcept
{
	for (auto it = _toWriteUdp.begin(); it != _toWriteUdp.end(); ++it) {
		if (it->first.getIndex() == pktIndex) {
			_toWriteUdp.erase(it);
			return;
		}
	}
}
} // namespace cf
