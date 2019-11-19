#pragma once

#include <Scene.hpp>
#include <Velocity.hpp>
#include <boost/asio.hpp>

#include "IGoWeapon.hpp"
#include "IGoEntity.hpp"
#include "CpnPrevPosition.hpp"
#include "Player.hpp"

namespace cf {

using boost::asio::ip::udp;

class GoPlayer : public IGoEntity, public Player {
      public:
	GoPlayer(const std::string &playerName, const Player &player) noexcept;
	void update(sfs::Scene &scene) noexcept;
	std::string asString() const noexcept;
	sfs::Velocity &getVelocity() noexcept;
	void setStaticSpeed() noexcept;
	void setFreeSpeed() noexcept;
	void collide(IGoEntity &entity) noexcept;
	void goToPrevPosition() noexcept;
	sf::FloatRect getHitBox() const noexcept;
	IGoWeapon *getWeapon() noexcept;

      private:
	IGoWeapon *_weapon;
	sfs::Velocity &_velocity;
	CpnPrevPosition &_prevPosition;
};
} // namespace cf
