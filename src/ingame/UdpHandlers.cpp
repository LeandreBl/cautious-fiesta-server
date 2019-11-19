#include <iostream>

#include <Trace.hpp>

#include "GoUdp.hpp"

namespace cf {
int GoUdp::positionHandler(GoPlayer &player, Serializer &toRead)
{
	uint32_t index;
	sf::Vector2f position;

	if (!toRead.get(index) || !toRead.get(position))
		return -1;
	std::cout << player.asString() << " " << toString(UdpPrctl::Type::POSITION) << " [" << index
		  << "] " << position << std::endl;
	return 0;
}

int GoUdp::velocityHandler(GoPlayer &player, Serializer &toRead)
{
	uint32_t index;
	sf::Vector2f speed;
	sf::Vector2f acceleration;

	if (!toRead.get(index) || !toRead.get(speed) || !toRead.get(acceleration))
		return -1;
	std::cout << player.asString() << " " << toString(UdpPrctl::Type::VELOCITY) << " [" << index
		  << "] " << speed << "p.s-1 " << acceleration << "p.s-2" << std::endl;
	return 0;
}

int GoUdp::spriteHandler(GoPlayer &player, Serializer &toRead)
{
	uint32_t index;
	sf::Vector2f offset;
	sf::Vector2f scale;
	float rotation;

	if (!toRead.get(index) || !toRead.get(offset) || !toRead.get(scale)
	    || !toRead.get(rotation))
		return -1;
	std::cout << player.asString() << " " << toString(UdpPrctl::Type::SPRITE) << " [" << index
		  << "] " << offset << "offset " << scale << "% " << rotation << "°" << std::endl;
	return 0;
}

int GoUdp::spawnHandler(GoPlayer &player, Serializer &toRead)
{
	int32_t type;
	uint32_t index;

	if (!toRead.get(type) || !toRead.get(index))
		return -1;
	std::cout << player.asString() << " " << toString(UdpPrctl::Type::SPAWN) << " [" << index
		  << "] " << type << std::endl;
	return 0;
}

static const sf::Vector2f MOVE_MATRIX[] = {
	{0, -10},
	{-10, 0},
	{0, 10},
	{10, 0},
};

int GoUdp::inputHandler(GoPlayer &player, Serializer &toRead)
{
	int32_t action;
	int32_t type;

	if (!toRead.get(action) || !toRead.get(type))
		return -1;
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
		case UdpPrctl::inputType::LEFT:
		case UdpPrctl::inputType::DOWN:
		case UdpPrctl::inputType::RIGHT:
			player.getVelocity().addSpeed(
				MOVE_MATRIX[type - static_cast<int>(UdpPrctl::inputType::UP)]);
			player.setStaticSpeed();
			break;
		}
		break;
	case UdpPrctl::inputAction::RELEASED:
		switch (static_cast<UdpPrctl::inputType>(type)) {
		case UdpPrctl::inputType::UP:
		case UdpPrctl::inputType::LEFT:
		case UdpPrctl::inputType::DOWN:
		case UdpPrctl::inputType::RIGHT:
			player.setFreeSpeed();
			break;
		}
	}
	return 0;
}

int GoUdp::timeHandler(GoPlayer &player, Serializer &toRead)
{
	int32_t type;

	if (!toRead.get(type))
		return -1;
	std::cout << player.asString() << " " << toString(UdpPrctl::Type::TIME) << type
		  << std::endl;
	return 0;
}

int GoUdp::unknownHandler(GoPlayer &player, Serializer &toRead)
{
	(void)toRead;
	std::cout << player.asString() << " " << toString(UdpPrctl::Type::UNKNOWN) << std::endl;
	return 0;
}

int GoUdp::ackHandler(GoPlayer &player, Serializer &toRead)
{
	(void)toRead;
	std::cout << player.asString() << " " << toString(UdpPrctl::Type::ACK) << std::endl;
	return 0;
}
} // namespace cf