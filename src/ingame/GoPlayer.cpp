#include "GoPlayer.hpp"

namespace cf {

GoPlayer::GoPlayer(const std::string &playerName, const Player &player) noexcept
	: Player(player)
	, _weapon(nullptr) /* TODO start with a weapon */
	, _velocity(addComponent<sfs::Velocity>(sf::Vector2f(0, 0), sf::Vector2f(0.5, 0.5)))
	, _prevPosition(addComponent<CpnPrevPosition>())
{
	name(playerName);
}

void GoPlayer::update(sfs::Scene &) noexcept
{
	if (getPosition() != _prevPosition.getPrevPosition())
		std::cout << asString() << std::endl;
}

std::string GoPlayer::asString() const noexcept
{
	return GameObject::asString() + " " + Player::asString();
}

sfs::Velocity &GoPlayer::getVelocity() noexcept
{
	return _velocity;
}

void GoPlayer::setStaticSpeed() noexcept
{
	_velocity.setAcceleration(sf::Vector2f(1, 1));
}

void GoPlayer::setFreeSpeed() noexcept
{
	_velocity.setAcceleration(sf::Vector2f(0.1, 0.1));
}

void GoPlayer::collide(IGoEntity &entity) noexcept
{
	/* TODO */
}

void GoPlayer::goToPrevPosition() noexcept
{
	setPosition(_prevPosition.getPrevPosition());
}

sf::FloatRect GoPlayer::getHitBox() const noexcept
{
	/* TODO with sprite size */
	return sf::FloatRect(getPosition(), sf::Vector2f(50, 50));
}

IGoWeapon *GoPlayer::getWeapon() noexcept
{
	return _weapon;
}

} // namespace cf