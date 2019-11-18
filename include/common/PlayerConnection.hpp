#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <queue>
#include <list>

#include <Serializer.hpp>
#include <Tcp.hpp>
#include <Udp.hpp>

#include "Player.hpp"

namespace cf {
/* forward declaration */
class GameRoom;
class Server;

using boost::asio::ip::tcp;
using boost::asio::ip::udp;

struct netBuffer {
	std::array<uint8_t, 1 << 12> buffer;
	Serializer serializer;
};

class PlayerConnection {
      public:
	PlayerConnection(std::unique_ptr<tcp::socket> &socket, Server &server) noexcept;
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
	void setPlayer(const Player::stats &stats, const sf::Color &color) noexcept;
	Player &getPlayer() noexcept;

	udp::endpoint &getUdpRemote() noexcept;
	Serializer &getUdpSerializer() noexcept;
	void pushUdpPacket(Serializer &packet, UdpPrctl::Type type) noexcept;
	void refreshUdp(udp::socket &socket) noexcept;
	UdpPrctl &getUdpHeader() noexcept;
	void setUdpHeader(const UdpPrctl::udpHeader &header) noexcept;
	void notifyUdpReceive(uint16_t pktIndex) noexcept;

      protected:
	void asyncReadHeader(const boost::system::error_code &error, std::size_t bytes_transferred);
	void asyncReadPayload(const boost::system::error_code &error,
			      std::size_t bytes_transferred);
	int writeTcp(const Serializer &serializer) noexcept;
	void headerMode() noexcept;
	void packetMode() noexcept;
	boost::asio::io_service _service;
	TcpPrctl _header;
	UdpPrctl _udpHeader;
	size_t _rd;
	struct netBuffer _netBuffer;
	size_t _toRead;
	std::string _name;
	udp::endpoint _udpRemote;
	std::unique_ptr<tcp::socket> _tcpSocket;
	GameRoom *_room;
	std::queue<Serializer> _toWrite;
	std::list<std::pair<UdpPrctl, Serializer>> _toWriteUdp;
	uint16_t _udpIndex;
	Serializer _udpSerializer;
	Server &_server;
	Player _player;
	bool _ready;
	bool _assetReady;
};
} // namespace cf