#include <math.h>

#include "Player.hpp"

namespace cf
{

struct Player::stats &operator+=(struct Player::stats &dest,
				 const struct Player::stats &src) noexcept
{
	dest.life += src.life;
	dest.speed += src.speed;
	dest.attack += src.attack;
	dest.attackSpeed += src.attackSpeed;
	dest.armor += src.armor;
	return dest;
}

Player::Player(const std::string &name, const struct stats &stats) noexcept
    : _name(name), _stats(stats)
{
}

const std::string &Player::getName() const noexcept
{
	return _name;
}

float Player::getLife() const noexcept
{
	return _stats.life;
}

void Player::inflictDamage(float amount) noexcept
{
	_stats.life -= amount * getArmorCoefficient();
	if (_stats.life < 0)
		_stats.life = 0.f;
}

float Player::getArmor() const noexcept
{
	return _stats.armor;
}

float Player::getArmorCoefficient() const noexcept
{
	return (100.f - log2(_stats.armor)) / 100;
}

float Player::getAttack() const noexcept
{
	return _stats.attack;
}

float Player::getAttackSpeed() const noexcept
{
	return _stats.attackSpeed;
}

float Player::getSpeed() const noexcept
{
	return _stats.speed;
}

bool Player::isAlive() const noexcept
{
	return _stats.life > 0;
}

void Player::levelUp(const struct stats &toAdd) noexcept
{
	_stats += toAdd;
}

std::ostream &operator<<(std::ostream &os, const Player &player) noexcept
{
	os << "\"" << player.getName() << "\": { life: " << player.getLife()
	   << ", speed: " << player.getSpeed()
	   << ", attack: " << player.getAttack()
	   << ", attackSpeed: " << player.getAttackSpeed()
	   << ", armor: " << player.getArmor() << " ("
	   << player.getArmorCoefficient() << "%) }";
	return os;
}

} // namespace cf