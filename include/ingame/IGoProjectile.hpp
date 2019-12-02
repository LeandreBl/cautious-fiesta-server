#pragma once

#include "GameManager.hpp"
#include "CpnPrevPosition.hpp"
#include "IGoEntity.hpp"

namespace cf {
class IGoProjectile : public IGoEntity {
      public:
	IGoProjectile(GameManager &manager, const sf::Vector2f &position, const sf::Vector2f &speed,
		      const sf::Vector2f &acceleration) noexcept;

	void goToPrevPosition() noexcept;
	virtual sf::FloatRect getHitBox() const noexcept = 0;
	virtual void collide(IGoEntity &entity) noexcept = 0;
	sfs::Velocity &getVelocity() noexcept;

      protected:
	GameManager &_manager;
	sfs::Velocity &_velocity;
	CpnPrevPosition &_prevPosition;
};
} // namespace cf