#include "GoPlayer.hpp"

namespace cf {

GoPlayer::GoPlayer(const std::string &playerName, const Player &player) noexcept
	: Player(player)
	, _name(playerName)
	, _velocity(addComponent<sfs::Velocity>(sf::Vector2f(0, 0), sf::Vector2f(0.7, 0.7)))
{
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
	_velocity.setAcceleration(sf::Vector2f(0.8, 0.8));
}

} // namespace cf