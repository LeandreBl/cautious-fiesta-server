#include <iostream>

#include <Trace.hpp>

#include "GoUdp.hpp"

namespace cf {
int GoUdp::positionHandler(GoPlayer &player, Serializer &s)
{
	uint32_t index;
	sf::Vector2f position;

	s >> index;
	s >> position;
	std::cout << player.asString() << " " << toString(UdpPrctl::Type::POSITION) << " [" << index
		  << "] " << position << std::endl;
	return 0;
}

int GoUdp::velocityHandler(GoPlayer &player, Serializer &s)
{
	uint32_t index;
	sf::Vector2f speed;
	sf::Vector2f acceleration;

	s >> index;
	s >> speed;
	s >> acceleration;
	std::cout << player.asString() << " " << toString(UdpPrctl::Type::VELOCITY) << " [" << index
		  << "] " << speed << "p.s-1 " << acceleration << "p.s-2" << std::endl;
	return 0;
}

int GoUdp::spriteHandler(GoPlayer &player, Serializer &s)
{
	uint32_t index;
	sf::Vector2f offset;
	sf::Vector2f scale;
	float rotation;

	s >> index;
	s >> offset;
	s >> scale;
	s >> rotation;
	std::cout << player.asString() << " " << toString(UdpPrctl::Type::SPRITE) << " [" << index
		  << "] " << offset << "offset " << scale << "% " << rotation << "°" << std::endl;
	return 0;
}

int GoUdp::spawnHandler(GoPlayer &player, Serializer &s)
{
	int32_t type;
	uint32_t index;

	s >> type;
	s >> index;
	std::cout << player.asString() << " " << toString(UdpPrctl::Type::SPAWN) << " [" << index
		  << "] " << type << std::endl;
	return 0;
}

int GoUdp::inputHandler(GoPlayer &player, Serializer &s)
{
	int32_t action;
	int32_t type;

	s >> action;
	s >> type;
	IGoWeapon *weapon;
	switch (static_cast<UdpPrctl::inputAction>(action)) {
	case UdpPrctl::inputAction::PRESSED:
		switch (static_cast<UdpPrctl::inputType>(type)) {
		case UdpPrctl::inputType::ATTACK1:
			weapon = player.getWeapon();
			if (weapon != nullptr) {
				weapon->attack(player);
			}
			break;
		case UdpPrctl::inputType::ATTACK2:
			weapon = player.getWeapon();
			if (weapon != nullptr) {
				weapon->specialAttack(player);
			}
			break;
		case UdpPrctl::inputType::UP:
			player.getVelocity().speed.y = -player.getSpeed() * 10;
			player.getVelocity().acceleration.y = 1;
			break;
		case UdpPrctl::inputType::LEFT:
			player.getVelocity().speed.x = -player.getSpeed() * 10;
			player.getVelocity().acceleration.x = 1;
			break;
		case UdpPrctl::inputType::DOWN:
			player.getVelocity().speed.y = player.getSpeed() * 10;
			player.getVelocity().acceleration.y = 1;
			break;
		case UdpPrctl::inputType::RIGHT:
			player.getVelocity().speed.x = player.getSpeed() * 10;
			player.getVelocity().acceleration.x = 1;
			break;
		}
		break;
	case UdpPrctl::inputAction::RELEASED:
		switch (static_cast<UdpPrctl::inputType>(type)) {
		case UdpPrctl::inputType::UP:
			player.getVelocity().acceleration.y = 0;
			break;
		case UdpPrctl::inputType::LEFT:
			player.getVelocity().acceleration.x = 0;
			break;
		case UdpPrctl::inputType::DOWN:
			player.getVelocity().acceleration.y = 0;
			break;
		case UdpPrctl::inputType::RIGHT:
			player.getVelocity().acceleration.x = 0;
			break;
		}
	}
	return 0;
}

int GoUdp::timeHandler(GoPlayer &player, Serializer &s)
{
	int32_t type;

	s >> type;
	std::cout << player.asString() << " " << toString(UdpPrctl::Type::TIME) << type
		  << std::endl;
	return 0;
}

int GoUdp::stateHandler(GoPlayer &player, Serializer &s)
{
	int32_t type;
	uint64_t id;

	s >> type;
	s >> id;
	std::cout << player.asString() << " " << toString(UdpPrctl::Type::STATE) << " " << id
		  << type << std::endl;
	return 0;
}

int GoUdp::destroyHandler(GoPlayer &player, Serializer &s)
{
	int32_t type;
	uint64_t id;

	s >> type;
	s >> id;
	std::cout << player.asString() << " " << toString(UdpPrctl::Type::DESTROY) << " " << id
		  << type << std::endl;
	return 0;
}

int GoUdp::attackHandler(GoPlayer &player, Serializer &s)
{
	int32_t type;
	int32_t attack;

	s >> type;
	s >> attack;
	std::cout << player.asString() << " " << toString(UdpPrctl::Type::ATTACK) << " " << attack
		  << type << std::endl;
	return 0;
}

int GoUdp::unknownHandler(GoPlayer &player, Serializer &s)
{
	(void)s;
	std::cout << player.asString() << " " << toString(UdpPrctl::Type::UNKNOWN) << std::endl;
	return 0;
}

int GoUdp::ackHandler(GoPlayer &player, Serializer &s)
{
	(void)s;
	std::cout << player.asString() << " " << toString(UdpPrctl::Type::ACK) << std::endl;
	return 0;
}
} // namespace cf