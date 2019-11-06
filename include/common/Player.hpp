#pragma once

#include <string>
#include <iostream>

namespace cf
{
class Player
{
      public:
	struct stats {
		float life;
		float speed;
		float attack;
		float attackSpeed;
		float armor;
	};
	struct stats &operator+=(const struct stats &src) noexcept;
	Player(const std::string &name = "",
	       const struct stats &stats = {0.f, 0.f, 0.f, 0.f, 0.f}) noexcept;
	const std::string &getName() const noexcept;
	float getLife() const noexcept;
	void inflictDamage(float amount) noexcept;
	float getArmor() const noexcept;
	float getArmorCoefficient() const noexcept;
	float getAttack() const noexcept;
	float getAttackSpeed() const noexcept;
	float getSpeed() const noexcept;
	bool isAlive() const noexcept;
	void levelUp(const struct stats &toAdd) noexcept;

      protected:
	std::string _name;
	struct stats _stats;
};

std::ostream &operator<<(std::ostream &os, const Player &player) noexcept;
} // namespace cf