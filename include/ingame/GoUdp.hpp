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
	GoUdp(GameManager &manager, boost::asio::io_service &service) noexcept;
	void start(sfs::Scene &scene) noexcept;
	void update(sfs::Scene &scene) noexcept;

      private:
	PlayerConnection *getConnectionFromEndpoint(const tcp::endpoint &endpoint) const noexcept;
	GoPlayer *getPlayerFromConnection(PlayerConnection &connection) const noexcept;
	void onUpdate(PlayerConnection &connection, const boost::system::error_code &error,
		      std::size_t bytes_transferred);
	void executePackets(PlayerConnection &connection) noexcept;
	void asyncReceive(PlayerConnection &connection) noexcept;
	void pollPackets() noexcept;
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
	int stateHandler(GoPlayer &player, Serializer &toRead);
	int destroyHandler(GoPlayer &player, Serializer &toRead);
	int attackHandler(GoPlayer &player, Serializer &toRead);
	int unknownHandler(GoPlayer &player, Serializer &toRead);
	int ackHandler(GoPlayer &player, Serializer &toRead);
	/* private data */
	std::function<int(GoPlayer &, Serializer &toRead)>
		_callbacks[static_cast<int>(UdpPrctl::Type::ACK) + 1];
	boost::asio::io_service &_ingameService;
	GameManager &_manager;
};
} // namespace cf