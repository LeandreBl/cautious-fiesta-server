#include "GoGun.hpp"
#include "GoBullet.hpp"

namespace cf {
GoGun::GoGun(GameManager &manager) noexcept
	: _manager(manager)
	, _prevAttack(0)
	, _prevSpecialAttack(0)
	, _cooldownCoef(0)
	, _spriteName("assets/GUN_30x30.png")
	, _sprite(nullptr)
{
}

void GoGun::spawn(GoPlayer &player) noexcept
{
	Serializer s;

	s << static_cast<int32_t>(UdpPrctl::spawnType::WEAPON);
	s << static_cast<uint64_t>(getId());
	s << static_cast<uint64_t>(player.getId());
	s << static_cast<int32_t>(UdpPrctl::weaponType::GUN);
	s << _spriteName;
	_manager.updateUdp(s, UdpPrctl::Type::SPAWN);
}

void GoGun::start(sfs::Scene &scene) noexcept
{
	auto *gunTexture = scene.getAssetTexture(_spriteName);

	if (gunTexture == nullptr) {
		errorLog("Can't load: " + _spriteName);
		destroy();
		return;
	}
	_sprite = &addComponent<sfs::Sprite>(*gunTexture);
}

void GoGun::attack(sfs::Scene &scene, GoPlayer &player, const sf::Vector2f &target) noexcept
{
	auto t = scene.realTime();

	if (t - _prevAttack > (_attCoef / player.getAttackSpeed()) * (1 - _cooldownCoef)) {
		Serializer s;
		s << static_cast<int32_t>(UdpPrctl::attackType::DEFAULT);
		s << target.x << target.y;
		_manager.updateUdp(s, UdpPrctl::Type::ATTACK);
		_prevAttack = t;
		player.addChild<GoBullet>(scene, _manager, player.getPosition(),
					  sf::Vector2f(100, 0));
	}
}

void GoGun::specialAttack(sfs::Scene &scene, GoPlayer &player, const sf::Vector2f &target) noexcept
{
	auto t = scene.realTime();

	if (t - _prevSpecialAttack
	    > (_speAttCoef / player.getAttackSpeed()) * (1 - _cooldownCoef)) {
		std::cout << t << "special attack" << target << std::endl;
		Serializer s;
		s << UdpPrctl::attackType::SPECIAL;
		s << target.x << target.y;
		_manager.updateUdp(s, UdpPrctl::Type::ATTACK);
		_prevSpecialAttack = t;
	}
}

void GoGun::setCooldownReduction(float coeff) noexcept
{
	_cooldownCoef = coeff;
}

float GoGun::getCooldownReduction() const noexcept
{
	return _cooldownCoef;
}

void GoGun::defaultCooldowns() noexcept
{
	_cooldownCoef = 0;
}

} // namespace cf