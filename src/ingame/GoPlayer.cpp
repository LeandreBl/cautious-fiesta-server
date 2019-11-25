#include "GoPlayer.hpp"
#include "GameManager.hpp"

namespace cf {

void GoPlayer::destroy() noexcept
{
	Serializer s;

	s << getId();
	_gameManager.updateUdp(s, UdpPrctl::Type::DESTROY);
	_gameManager.getColliderManager().removeFromAllies(*this);
}

GoPlayer::GoPlayer(const sf::Vector2f &position, GameManager &manager,
		   const std::string &playerName, const Stats &player) noexcept
	: IGoEntity(position, playerName, player)
	, _gameManager(manager)
	, _weapon(nullptr) /* TODO start with a weapon */
	, _velocity(addComponent<sfs::Velocity>(sf::Vector2f(0, 0), sf::Vector2f(0, 0)))
	, _prevPosition(addComponent<CpnPrevPosition>())
	, _hat(addComponent<sfs::Sprite>())
	, _spriteName("assets/HAT_61x61.png")
{
	manager.updateUdp(serialize(), cf::UdpPrctl::Type::SPAWN);
}

void GoPlayer::start(sfs::Scene &scene) noexcept
{
	auto *texture = scene.getAssetTexture(_spriteName);
	if (texture == nullptr) {
		std::cerr << "Unable to find " << _spriteName << " asset"
			  << "\r";
		std::cerr.flush();
	}
	else {
		_hat.setTexture(*texture, true);
	}
	_gameManager.getColliderManager().addToAllies(*this);
}

void GoPlayer::update(sfs::Scene &) noexcept
{
	if (getPosition() != _prevPosition.getPrevPosition()) {
		Serializer s;
		s << getId() << getPosition().x << getPosition().y;
		_gameManager.updateUdp(s, UdpPrctl::Type::POSITION);
		s.clear();
		s << getId() << _velocity.speed.x << _velocity.speed.y << _velocity.acceleration.x
		  << _velocity.acceleration.y;
		_gameManager.updateUdp(s, UdpPrctl::Type::VELOCITY);
	}
}

std::string GoPlayer::asString() const noexcept
{
	return GameObject::asString() + " " + Stats::asString();
}

sfs::Velocity &GoPlayer::getVelocity() noexcept
{
	return _velocity;
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

	s << static_cast<int32_t>(UdpPrctl::spawnType::PLAYER) << getId() << getName() << getLife()
	  << getSpeed() << getAttack() << getAttackSpeed() << getArmor() << getColor()
	  << _spriteName << static_cast<int32_t>(UdpPrctl::weaponType::NONE);
	return s;
}

} // namespace cf