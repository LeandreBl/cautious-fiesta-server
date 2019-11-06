#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <queue>

#include <Serializer.hpp>
#include <Tcp.hpp>
#include <Udp.h>

#include "Player.hpp"

namespace cf
{
/* forward declaration */
class GameRoom;
class Server;

using boost::asio::ip::tcp;
using boost::asio::ip::udp;

class PlayerConnection
{
      public:
	PlayerConnection(std::unique_ptr<tcp::socket> &socket,
			 Server &server) noexcept;
	~PlayerConnection() = default;

	void name(const std::string &name) noexcept;
	const std::string &name() const noexcept;
	void ready(bool state) noexcept;
	bool ready() const noexcept;
	void assetReady(bool state) noexcept;
	bool isLogged() const noexcept;
	bool isInRoom() const noexcept;
	GameRoom &room() const noexcept;
	void room(GameRoom &room) noexcept;
	void leaveRoom() noexcept;
	int getId() const noexcept;
	std::string getIp() const noexcept;
	void close() noexcept;
	void pushPacket(Serializer &packet, TcpPrctl::Type type) noexcept;
	void refreshTcp() noexcept;
	void setUdpPort(uint16_t port) noexcept;
	void pushUdp(Serializer &packet, UdpPrctl::Type type) noexcept;
	void refreshUdp() noexcept;
	void setPlayer(const Player::stats &stats) noexcept;
	Player &getPlayer() noexcept;
      protected:
	void asyncReadHeader(const boost::system::error_code &error,
			     std::size_t bytes_transferred);
	void asyncReadPayload(const boost::system::error_code &error,
			      std::size_t bytes_transferred);
	int writeTcp(const Serializer &serializer) noexcept;
	int writeUdp(const Serializer &serializer) noexcept;
	void headerMode() noexcept;
	void packetMode() noexcept;
	boost::asio::io_service _service;
	TcpPrctl _header;
	size_t _rd;
	std::array<uint8_t, 4096> _buffer;
	size_t _toRead;
	Serializer _payload;
	std::string _name;
	std::unique_ptr<udp::socket> _udpSocket;
	uint16_t _udpIndex;
	udp::endpoint _udpRemote;
	std::unique_ptr<tcp::socket> _tcpSocket;
	GameRoom *_room;
	std::queue<Serializer> _toWrite;
	std::queue<Serializer> _toWriteUdp;
	Server &_server;
	Player _player;
	bool _ready;
	bool _assetReady;
};
} // namespace cf