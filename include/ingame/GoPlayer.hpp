#pragma once

#include <Scene.hpp>
#include <boost/asio.hpp>

#include "IGoWeapon.hpp"
#include "IGoEntity.hpp"
#include "CpnPrevPosition.hpp"
#include "Stats.hpp"

namespace cf {

using boost::asio::ip::udp;
class GameManager;

class GoPlayer : public IGoEntity {
      public:
	~GoPlayer();
	GoPlayer(const sf::Vector2f &position, GameManager &manager, const std::string &playerName,
		 const Stats &player) noexcept;
	void start(sfs::Scene &scene) noexcept;
	void update(sfs::Scene &scene) noexcept;
	std::string asString() const noexcept;
	sfs::Velocity &getVelocity() noexcept;
	void setStaticSpeed() noexcept;
	void setFreeSpeed() noexcept;
	void collide(IGoEntity &entity) noexcept;
	void goToPrevPosition() noexcept;
	sf::FloatRect getHitBox() const noexcept;
	IGoWeapon *getWeapon() noexcept;
	Serializer serialize() const noexcept;

      private:
	GameManager &_gameManager;
	IGoWeapon *_weapon;
	sfs::Velocity &_velocity;
	CpnPrevPosition &_prevPosition;
};
} // namespace cf
