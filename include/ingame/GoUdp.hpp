#pragma once

#include <queue>

#include <boost/asio.hpp>
#include <GameObject.hpp>

#include "GameManager.hpp"
#include "PlayerConnection.hpp"
#include "GoPlayer.hpp"

namespace cf {

using boost::asio::ip::udp;

class GoUdp : public sfs::GameObject {
      public:
	GoUdp(GameManager &manager, uint16_t port) noexcept;
	void start(sfs::Scene &scene) noexcept;
	void update(sfs::Scene &scene) noexcept;

      private:
	GoPlayer *getPlayerFromConnection(PlayerConnection &connection) const noexcept;
	PlayerConnection *getClient(const udp::endpoint &remote) const noexcept;
	void onUpdate(const boost::system::error_code &error, std::size_t bytes_transferred);
	void executePackets() noexcept;
	void asyncReceive() noexcept;
	template <typename... Args> void autoBind(UdpPrctl::Type type, Args... args)
	{
		_callbacks[static_cast<int>(type)] =
			std::bind(args..., this, std::placeholders::_1, std::placeholders::_2);
	}
	/* udp handlers */
	int positionHandler(GoPlayer &player, Serializer &toRead);
	int velocityHandler(GoPlayer &player, Serializer &toRead);
	int spriteHandler(GoPlayer &player, Serializer &toRead);
	int spawnHandler(GoPlayer &player, Serializer &toRead);
	int inputHandler(GoPlayer &player, Serializer &toRead);
	int timeHandler(GoPlayer &player, Serializer &toRead);
	int unknownHandler(GoPlayer &player, Serializer &toRead);
	int ackHandler(GoPlayer &player, Serializer &toRead);
	/* private data */
	std::function<int(GoPlayer &, Serializer &toRead)>
		_callbacks[static_cast<int>(UdpPrctl::Type::ACK) + 1];
	boost::asio::io_service _udpService;
	udp::endpoint _lastRemote;
	std::array<uint8_t, 1 << 12> _commonBuffer;
	udp::socket _commonSocket;
	GameManager &_manager;
	std::queue<std::pair<UdpPrctl, PlayerConnection *>> _toProcess;
};
} // namespace cf