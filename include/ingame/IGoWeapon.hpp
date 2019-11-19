#pragma once

#include <GameObject.hpp>

namespace cf {
class GoPlayer;
class IGoWeapon : public sfs::GameObject {
      public:
	virtual void attack(GoPlayer &player) noexcept = 0;
	virtual void specialAttack(GoPlayer &player) noexcept = 0;
	virtual float getAttackCooldown() const noexcept = 0;
	virtual float getSpecialAttackCooldown() const noexcept = 0;
	virtual void setCooldownReduction(float percentage) const noexcept = 0;
	virtual float getCooldownReduction() const noexcept = 0;
	virtual void defaultCooldowns() const noexcept = 0;
};
} // namespace cf