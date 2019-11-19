#pragma once

#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>

namespace cf {
class Stats {
      public:
	struct stats {
		float life;
		float speed;
		float attack;
		float attackSpeed;
		float armor;
	};
	struct stats &operator+=(const struct stats &src) noexcept;
	Stats(const std::string &name = "", const struct stats &stats = {0.f, 0.f, 0.f, 0.f, 0.f},
	      const sf::Color &color = sf::Color()) noexcept;
	const std::string &getName() const noexcept;
	float getLife() const noexcept;
	void setLife(float life) noexcept;
	void inflictDamage(float amount) noexcept;
	float getArmor() const noexcept;
	void setArmor(float armor) noexcept;
	float getArmorCoefficient() const noexcept;
	float getAttack() const noexcept;
	void setAttack(float attack) noexcept;
	float getAttackSpeed() const noexcept;
	void setAttackSpeed(float attackSpeed) noexcept;
	float getSpeed() const noexcept;
	void setSpeed(float speed) noexcept;
	const sf::Color &getColor() const noexcept;
	void setColor(const sf::Color &color) noexcept;
	bool isAlive() const noexcept;
	void levelUp(const struct stats &toAdd) noexcept;
	std::string asString() const noexcept;

      protected:
	std::string _name;
	struct stats _stats;
	sf::Color _color;
};
} // namespace cf