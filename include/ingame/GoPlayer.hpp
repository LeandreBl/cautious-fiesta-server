#pragma once

#include <Scene.hpp>
#include <GameObject.hpp>
#include <boost/asio.hpp>

#include "Player.hpp"

namespace cf {

using boost::asio::ip::udp;

class GoPlayer : public sfs::GameObject, public Player {
      public:
	GoPlayer(const std::string &playerName, const Player &player) noexcept;

      private:
	std::string _name;
};
} // namespace cf