#include "GoPlayer.hpp"
#include "GameManager.hpp"

namespace cf {

GoPlayer::~GoPlayer() noexcept
{
	_gameManager.getColliderManager().removeFromAllies(*this);
}

GoPlayer::GoPlayer(const sf::Vector2f &position, GameManager &manager,
		   const std::string &playerName, const Stats &player) noexcept
	: IGoEntity(position, playerName, player)
	, _gameManager(manager)
	, _weapon(nullptr) /* TODO start with a weapon */
	, _velocity(addComponent<sfs::Velocity>(sf::Vector2f(0, 0), sf::Vector2f(0.5, 0.5)))
	, _prevPosition(addComponent<CpnPrevPosition>())
{
	manager.updateUdp(serialize(), cf::UdpPrctl::Type::SPAWN);
}

void GoPlayer::start(sfs::Scene &) noexcept
{
	_gameManager.getColliderManager().addToAllies(*this);
}

void GoPlayer::update(sfs::Scene &) noexcept
{
	if (getPosition() != _prevPosition.getPrevPosition())
		std::cout << asString() << std::endl;
	
}

std::string GoPlayer::asString() const noexcept
{
	return GameObject::asString() + " " + Stats::asString();
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

Serializer GoPlayer::serialize() const noexcept
{
	Serializer s;

	s.set(static_cast<int32_t>(UdpPrctl::objType::PLAYER));
	s.set(getName());
	s.set(getLife());
	s.set(getSpeed());
	s.set(getAttack());
	s.set(getAttackSpeed());
	s.set(getArmor());
	s.set(getColor());
	s.set(static_cast<int32_t>(UdpPrctl::weaponType::NONE));
	return s;
}

} // namespace cf