#pragma once

#include <GameObject.hpp>
#include <Velocity.hpp>
#include <Serializer.hpp>

#include "Stats.hpp"

namespace cf {
class IGoObstacle;
class IGoEntity : public sfs::GameObject, public Stats {
      public:
	IGoEntity(const sf::Vector2f &position = sf::Vector2f(0, 0), const std::string &name = "",
		  const Stats &player = Stats()) noexcept
		: sfs::GameObject(position, name)
		, Stats(player)
	{
	}
	enum EntityType { BULLET, PLAYER, ENNEMY };
	virtual void goToPrevPosition() noexcept = 0;
	virtual sf::FloatRect getHitBox() const noexcept = 0;
	virtual void collide(IGoEntity &entity) noexcept = 0;
	virtual void collide(IGoObstacle &obstacle) noexcept = 0;
	virtual sfs::Velocity &getVelocity() noexcept = 0;
	virtual EntityType getEntityType() noexcept = 0;
};
} // namespace cf
