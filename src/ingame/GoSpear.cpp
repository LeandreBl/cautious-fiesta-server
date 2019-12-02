#include "GoSpear.hpp"

namespace cf
{
GoSpear::GoSpear(GameManager &manager) noexcept
    : _manager(manager),
      _prevAttack(0),
      _prevSpecialAttack(0),
      _cooldownCoef(0),
      _spriteName("assets/SPEAR_53x53.png"),
      _sprite(nullptr)
{
}

void GoSpear::spawn(GoPlayer &player) noexcept
{
    Serializer s;

    s << static_cast<int32_t>(UdpPrctl::spawnType::WEAPON);
    s << static_cast<uint64_t>(getId());
    s << static_cast<uint64_t>(player.getId());
    s << static_cast<int32_t>(UdpPrctl::weaponType::SPEAR);
    s << _spriteName;
    _manager.updateUdp(s, UdpPrctl::Type::SPAWN);
}

void GoSpear::start(sfs::Scene &scene) noexcept
{
    auto *spearTexture = scene.getAssetTexture(_spriteName);

    if (spearTexture == nullptr)
    {
        errorLog("Can't load: " + _spriteName);
        destroy();
        return;
    }
    _sprite = &addComponent<sfs::Sprite>(*spearTexture);
}

void GoSpear::attack(sfs::Scene &scene, GoPlayer &player, const sf::Vector2f &target) noexcept
{
    auto t = scene.realTime();

    if (t - _prevAttack > (_attCoef / player.getAttackSpeed()) * (1 - _cooldownCoef))
    {
        std::cout << t << "default attack" << target << std::endl;
        Serializer s;
        s << static_cast<int32_t>(UdpPrctl::attackType::DEFAULT);
        s << target.x << target.y;
        _manager.updateUdp(s, UdpPrctl::Type::ATTACK);
        _prevAttack = t;
    }
}

void GoSpear::specialAttack(sfs::Scene &scene, GoPlayer &player, const sf::Vector2f &target) noexcept
{
    auto t = scene.realTime();

    if (t - _prevSpecialAttack > (_speAttCoef / player.getAttackSpeed()) * (1 - _cooldownCoef))
    {
        std::cout << t << "special attack" << target << std::endl;
        Serializer s;
        s << UdpPrctl::attackType::SPECIAL;
        s << target.x << target.y;
        _manager.updateUdp(s, UdpPrctl::Type::ATTACK);
        _prevSpecialAttack = t;
    }
}

void GoSpear::setCooldownReduction(float coeff) noexcept
{
    _cooldownCoef = coeff;
}

float GoSpear::getCooldownReduction() const noexcept
{
    return _cooldownCoef;
}

void GoSpear::defaultCooldowns() noexcept
{
    _cooldownCoef = 0;
}

} // namespace cf