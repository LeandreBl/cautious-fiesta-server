#pragma once

#include <Scene.hpp>
#include <GameObject.hpp>
#include <Velocity.hpp>
#include <boost/asio.hpp>

#include "Player.hpp"

namespace cf {

using boost::asio::ip::udp;

class GoPlayer : public sfs::GameObject, public Player {
      public:
	GoPlayer(const std::string &playerName, const Player &player) noexcept;
	std::string asString() const noexcept;
	sfs::Velocity &getVelocity() noexcept;
	void setStaticSpeed() noexcept;
	void setFreeSpeed() noexcept;

      private:
	std::string _name;
	sfs::Velocity &_velocity;
};
} // namespace cf
