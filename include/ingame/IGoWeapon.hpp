#pragma once

#include <GameObject.hpp>

namespace cf
{
class GoPlayer;
class IGoWeapon : public sfs::GameObject
{
public:
	virtual void spawn(GoPlayer &player) noexcept = 0;
	virtual void attack(sfs::Scene &scene, GoPlayer &player, const sf::Vector2f &target) noexcept = 0;
	virtual void specialAttack(sfs::Scene &scene, GoPlayer &player, const sf::Vector2f &target) noexcept = 0;
	virtual void setCooldownReduction(float coeff) noexcept = 0;
	virtual float getCooldownReduction() const noexcept = 0;
	virtual void defaultCooldowns() noexcept = 0;
};
} // namespace cf