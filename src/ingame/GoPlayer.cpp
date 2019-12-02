#include "GoPlayer.hpp"
#include "GameManager.hpp"
#include "GoDagger.hpp"
#include "GoSpear.hpp"
#include "GoSword.hpp"
#include "GoGun.hpp"

namespace cf
{

void GoPlayer::destroy() noexcept
{
	Serializer s;

	s << getId();
	_gameManager.updateUdp(s, UdpPrctl::Type::DESTROY);
	_gameManager.getColliderManager().removeFromAllies(*this);
}

GoPlayer::GoPlayer(const sf::Vector2f &position, GameManager &manager,
				   const std::string &playerName, const Stats &player) noexcept
	: IGoEntity(position, playerName, player), _gameManager(manager), _weapon(nullptr) /* TODO start with a weapon */
	  ,
	  _velocity(addComponent<sfs::Velocity>(sf::Vector2f(0, 0), sf::Vector2f(0, 0))), _prevPosition(addComponent<CpnPrevPosition>()), _hat(addComponent<sfs::Sprite>()), _spriteName("assets/HAT_61x61.png")

{
	_keyMap[UdpPrctl::inputType::UP] = UdpPrctl::inputAction::RELEASED;
	_keyMap[UdpPrctl::inputType::DOWN] = UdpPrctl::inputAction::RELEASED;
	_keyMap[UdpPrctl::inputType::LEFT] = UdpPrctl::inputAction::RELEASED;
	_keyMap[UdpPrctl::inputType::RIGHT] = UdpPrctl::inputAction::RELEASED;
	_keyMap[UdpPrctl::inputType::ATTACK1] = UdpPrctl::inputAction::RELEASED;
	_keyMap[UdpPrctl::inputType::ATTACK2] = UdpPrctl::inputAction::RELEASED;
}

void GoPlayer::start(sfs::Scene &scene) noexcept
{
	auto *texture = scene.getAssetTexture(_spriteName);
	if (texture == nullptr)
	{
		std::cerr << "Unable to find " << _spriteName << " asset"
				  << "\r";
		std::cerr.flush();
	}
	else
	{
		_hat.setTexture(*texture, true);
	}
	_gameManager.getColliderManager().addToAllies(*this);
	_gameManager.updateUdp(serialize(), cf::UdpPrctl::Type::SPAWN);
	_weapon = static_cast<IGoWeapon *>(&addChild<GoSword>(scene, _gameManager));
	_weapon->spawn(*this);
}

void GoPlayer::update(sfs::Scene &) noexcept
{
}

static UdpPrctl::inputType reverseMove(UdpPrctl::inputType key)
{
	switch (key)
	{
	case UdpPrctl::inputType::UP:
		return UdpPrctl::inputType::DOWN;
	case UdpPrctl::inputType::DOWN:
		return UdpPrctl::inputType::UP;
	case UdpPrctl::inputType::LEFT:
		return UdpPrctl::inputType::RIGHT;
	case UdpPrctl::inputType::RIGHT:
		return UdpPrctl::inputType::LEFT;
	default:
		return UdpPrctl::inputType::UNKNOWN_KEY;
	}
}

static void setValue(sf::Vector2f &s, sf::Vector2f &a, UdpPrctl::inputType type, float value)
{
	switch (type)
	{
	case UdpPrctl::inputType::UP:
		s.y = -value;
		a.y = !!value;
		break;
	case UdpPrctl::inputType::DOWN:
		s.y = value;
		a.y = !!value;
		break;
	case UdpPrctl::inputType::LEFT:
		s.x = -value;
		a.x = !!value;
		break;
	case UdpPrctl::inputType::RIGHT:
		s.x = value;
		a.x = !!value;
		break;
	}
}

void GoPlayer::updateMovementMatrix(UdpPrctl::inputType key, UdpPrctl::inputAction action) noexcept
{
	sf::Vector2f speed = _velocity.speed;
	sf::Vector2f acceleration = _velocity.acceleration;

	auto k = _keyMap[key];
	_keyMap[key] = action;
	if (k == action)
		return;
	k = action;
	auto keyp = reverseMove(key);
	auto kp = _keyMap[keyp];
	const float coef = getSpeed() * 10;
	if (k == UdpPrctl::inputAction::PRESSED)
	{
		if (kp == UdpPrctl::inputAction::PRESSED)
		{
			setValue(_velocity.speed, _velocity.acceleration, key, 0);
		}
		else if (kp == UdpPrctl::inputAction::RELEASED)
		{
			setValue(_velocity.speed, _velocity.acceleration, key, coef);
		}
	}
	else if (k == UdpPrctl::inputAction::RELEASED)
	{
		if (kp == UdpPrctl::inputAction::PRESSED)
		{
			setValue(_velocity.speed, _velocity.acceleration, keyp, coef);
		}
		else if (kp == UdpPrctl::inputAction::RELEASED)
		{
			setValue(_velocity.speed, _velocity.acceleration, keyp, 0);
		}
	}
	/* if state changed, update NET */
	if (speed != _velocity.speed || acceleration != _velocity.acceleration)
	{
		updateUdpPosition();
		updateUdpVelocity();
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

void GoPlayer::updateUdpPosition() noexcept
{
	Serializer s;
	s << getId() << getPosition().x << getPosition().y;
	_gameManager.updateUdp(s, UdpPrctl::Type::POSITION);
}

void GoPlayer::updateUdpVelocity() noexcept
{
	Serializer s;
	s << getId() << _velocity.speed.x << _velocity.speed.y << _velocity.acceleration.x
	  << _velocity.acceleration.y;
	_gameManager.updateUdp(s, UdpPrctl::Type::VELOCITY);
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