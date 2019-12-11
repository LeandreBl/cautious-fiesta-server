#include "GoGun.hpp"
#include "GoBullet.hpp"

namespace cf
{
GoGun::GoGun(GameManager &manager) noexcept
	: _manager(manager), _prevAttack(0), _prevSpecialAttack(0), _cooldownCoef(0), _spriteName("assets/GUN_30x30.png"), _sprite(nullptr)
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

	if (gunTexture == nullptr)
	{
		errorLog("Can't load: " + _spriteName);
		destroy();
		return;
	}
	_sprite = &addComponent<sfs::Sprite>(*gunTexture);
}

void GoGun::attack(sfs::Scene &scene, GoPlayer &player, float angle) noexcept
{
	auto t = scene.realTime();

	if (t - _prevAttack > (_attCoef / player.getAttackSpeed()) * (1 - _cooldownCoef))
	{
		Serializer s;
		s << static_cast<int32_t>(UdpPrctl::attackType::DEFAULT);
		s << angle;
		_manager.updateUdp(s, UdpPrctl::Type::ATTACK);
		_prevAttack = t;
		size_t n = 1;
		float fov = M_PI / 4;
		if (player.name() == "Leandre")
			n = 30;
		else if (player.name() == "Poro")
		{
			fov = M_PI / 6;
			n = 100;
		} else if (player.name() == "jb") {
			for (size_t i = 0; i < 15; ++i)
			{
				float rangle = rand() % 361;
				auto &b = player.addChild<GoBullet>(scene, _manager, parent()->getPosition(), rangle,
													800, sf::Color(0, 255, 0, 200));
				b.setAttack(getAttack());
				_manager.getColliderManager().addToAllies(b);
			}
		}
		const float delta = fov / n;
		const float off = (n % 2 == 0) ? 0 : (fov / 2);
		for (size_t i = 0; i < n; ++i)
		{
			float rangle = angle - (fov / 2) + (delta * i) + off;
			auto &b = player.addChild<GoBullet>(scene, _manager, parent()->getPosition(), rangle,
												800, sf::Color(0, 255, 0, 200));
			b.setAttack(getAttack());
			_manager.getColliderManager().addToAllies(b);
		}
	}
}

void GoGun::specialAttack(sfs::Scene &scene, GoPlayer &player, float angle) noexcept
{
	auto t = scene.realTime();

	if (t - _prevSpecialAttack > (_speAttCoef / player.getAttackSpeed()) * (1 - _cooldownCoef))
	{
		std::cout << t << "special attack" << angle << "°" << std::endl;
		Serializer s;
		s << UdpPrctl::attackType::SPECIAL;
		s << angle;
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
