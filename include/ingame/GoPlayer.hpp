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
	void destroy() noexcept;
	GoPlayer(const sf::Vector2f &position, GameManager &manager, const std::string &playerName,
		 const Stats &player) noexcept;
	void start(sfs::Scene &scene) noexcept;
	void update(sfs::Scene &scene) noexcept;
	void updateMovementMatrix(UdpPrctl::inputType key, UdpPrctl::inputAction action) noexcept;
	void updateUdpPosition() noexcept;
	void updateUdpVelocity() noexcept;
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
	std::unordered_map<UdpPrctl::inputType, UdpPrctl::inputAction> _keyMap;
	GameManager &_gameManager;
	IGoWeapon *_weapon;
	sfs::Velocity &_velocity;
	CpnPrevPosition &_prevPosition;
	sfs::Sprite &_hat;
	std::string _spriteName;
};
} // namespace cf
