#pragma once

#include <boost/asio.hpp>
#include <GameObject.hpp>

#include "GameRoom.hpp"
#include "GoPlayer.hpp"

namespace cf {
class GameManager : public sfs::GameObject {
      public:
	GameManager(GameRoom &room, uint16_t port) noexcept;
	void start(sfs::Scene &scene) noexcept;
	std::vector<GoPlayer *> &getGoPlayers() noexcept;
	const std::vector<PlayerConnection *> &getConnections() noexcept;

      private:
	uint16_t _udpPort;
	boost::asio::io_service _udpService;
	std::vector<GoPlayer *> _players;
	GameRoom &_gameRoom;
};
} // namespace cf