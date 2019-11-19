#include <math.h>

#include <sstream>

#include "Stats.hpp"

namespace cf {

struct Stats::stats &operator+=(struct Stats::stats &dest, const struct Stats::stats &src) noexcept
{
	dest.life += src.life;
	dest.speed += src.speed;
	dest.attack += src.attack;
	dest.attackSpeed += src.attackSpeed;
	dest.armor += src.armor;
	return dest;
}

Stats::Stats(const std::string &name, const struct stats &stats, const sf::Color &color) noexcept
	: _name(name)
	, _stats(stats)
	, _color(color)
{
}

const std::string &Stats::getName() const noexcept
{
	return _name;
}

float Stats::getLife() const noexcept
{
	return _stats.life;
}

void Stats::setLife(float life) noexcept
{
	_stats.life = life;
}

void Stats::inflictDamage(float amount) noexcept
{
	_stats.life -= amount * getArmorCoefficient();
	if (_stats.life < 0)
		_stats.life = 0.f;
}

float Stats::getArmor() const noexcept
{
	return _stats.armor;
}

void Stats::setArmor(float armor) noexcept
{
	_stats.armor = armor;
}

float Stats::getArmorCoefficient() const noexcept
{
	return (100.f - log2(_stats.armor)) / 100;
}

float Stats::getAttack() const noexcept
{
	return _stats.attack;
}

void Stats::setAttack(float attack) noexcept
{
	_stats.attack = attack;
}

float Stats::getAttackSpeed() const noexcept
{
	return _stats.attackSpeed;
}

void Stats::setAttackSpeed(float attackSpeed) noexcept
{
	_stats.attackSpeed = attackSpeed;
}

float Stats::getSpeed() const noexcept
{
	return _stats.speed;
}

void Stats::setSpeed(float speed) noexcept
{
	_stats.speed = speed;
}

const sf::Color &Stats::getColor() const noexcept
{
	return _color;
}

void Stats::setColor(const sf::Color &color) noexcept
{
	_color = color;
}

bool Stats::isAlive() const noexcept
{
	return _stats.life > 0;
}

void Stats::levelUp(const struct stats &toAdd) noexcept
{
	_stats += toAdd;
}

std::string Stats::asString() const noexcept
{
	std::ostringstream os;

	os << "\"" << _name << "\": { life: " << _stats.life << ", speed: " << _stats.speed
	   << ", attack: " << _stats.attack << ", attackSpeed: " << _stats.attackSpeed
	   << ", armor: " << _stats.armor << " (x" << getArmorCoefficient() << "%) }";
	return os.str();
}
} // namespace cf