#include "IGoProjectile.hpp"

namespace cf {
IGoProjectile::IGoProjectile(GameManager &manager, const sf::Vector2f &position,
			     const sf::Vector2f &speed, const sf::Vector2f &acceleration) noexcept
	: IGoEntity(position, "projectile")
	, _manager(manager)
	, _velocity(addComponent<sfs::Velocity>(speed, acceleration))
	, _prevPosition(addComponent<CpnPrevPosition>())
{
}

void IGoProjectile::goToPrevPosition() noexcept
{
	setPosition(_prevPosition.getPrevPosition());
}

sfs::Velocity &IGoProjectile::getVelocity() noexcept
{
	return _velocity;
}
} // namespace cf