#include "GoDagger.hpp"

namespace cf
{
GoDagger::GoDagger(GameManager &manager) noexcept
    : _manager(manager),
      _prevAttack(0),
      _prevSpecialAttack(0),
      _cooldownCoef(0),
      _spriteName("assets/DAGGER_20x20.png"),
      _sprite(nullptr)
{
}

void GoDagger::spawn(GoPlayer &player) noexcept
{
    Serializer s;

    s << static_cast<int32_t>(UdpPrctl::spawnType::WEAPON);
    s << static_cast<uint64_t>(getId());
    s << static_cast<uint64_t>(player.getId());
    s << static_cast<int32_t>(UdpPrctl::weaponType::DAGGER);
    s << _spriteName;
    _manager.updateUdp(s, UdpPrctl::Type::SPAWN);
}

void GoDagger::start(sfs::Scene &scene) noexcept
{
    auto *daggerTexture = scene.getAssetTexture(_spriteName);

    if (daggerTexture == nullptr)
    {
        errorLog("Can't load: " + _spriteName);
        destroy();
        return;
    }
    _sprite = &addComponent<sfs::Sprite>(*daggerTexture);
}

void GoDagger::attack(sfs::Scene &scene, GoPlayer &player, const sf::Vector2f &target) noexcept
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

void GoDagger::specialAttack(sfs::Scene &scene, GoPlayer &player, const sf::Vector2f &target) noexcept
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

void GoDagger::setCooldownReduction(float coeff) noexcept
{
    _cooldownCoef = coeff;
}

float GoDagger::getCooldownReduction() const noexcept
{
    return _cooldownCoef;
}

void GoDagger::defaultCooldowns() noexcept
{
    _cooldownCoef = 0;
}

} // namespace cf