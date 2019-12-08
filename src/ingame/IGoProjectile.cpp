#include "IGoProjectile.hpp"

namespace cf {
IGoProjectile::IGoProjectile(GameManager &manager, const sf::Vector2f &position, float angle,
			     float speed) noexcept
	: IGoEntity(position, "projectile")
	, _manager(manager)
	, _velocity(addComponent<sfs::Velocity>(
		  sf::Vector2f(cosf(angle) * -speed, sinf(angle) * -speed)))
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