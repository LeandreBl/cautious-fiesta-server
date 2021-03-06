#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <queue>
#include <list>
#include <array>

#include <Serializer.hpp>
#include <Tcp.hpp>
#include <Udp.hpp>

#include "Stats.hpp"

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
	void closeUdp() noexcept;
	void pushPacket(Serializer &packet, TcpPrctl::Type type) noexcept;
	void refreshTcp() noexcept;
	void setUdpPort(uint16_t port) noexcept;
	void refreshUdp() noexcept;
	void setPlayer(const Stats::stats &stats, const sf::Color &color) noexcept;
	Stats &getPlayer() noexcept;

	tcp::socket &setSocket(boost::asio::io_service &service) noexcept;
	tcp::socket &getIngameSocket() noexcept;
	void pushUdpPacket(const Serializer &packet, UdpPrctl::Type type) noexcept;
	struct netBuffer &getUdpBuffer() noexcept;

      protected:
	void asyncReadHeader(const boost::system::error_code &error, std::size_t bytes_transferred);
	void asyncReadPayload(const boost::system::error_code &error,
			      std::size_t bytes_transferred);
	int writeTcp(const Serializer &serializer) noexcept;
	void headerMode() noexcept;
	void packetMode() noexcept;
	const int _id;
	TcpPrctl _header;
	size_t _rd;
	struct netBuffer _netBuffer;
	size_t _toRead;
	std::string _name;
	std::unique_ptr<tcp::socket> _ingameSocket;
	struct netBuffer _udpBuffer;
	std::unique_ptr<tcp::socket> _tcpSocket;
	GameRoom *_room;
	std::queue<Serializer> _toWrite;
	std::queue<Serializer> _toWriteUdp;
	uint16_t _udpIndex;
	Server &_server;
	Stats _player;
	bool _ready;
	bool _assetReady;
};
} // namespace cf