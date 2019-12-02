#pragma once

#include <BasicShape.hpp>

#include "IGoWeapon.hpp"
#include "GoPlayer.hpp"
#include "GameManager.hpp"

namespace cf
{
class GoSword : public IGoWeapon
{
public:
    GoSword(GameManager &manager) noexcept;
    void start(sfs::Scene &scene) noexcept;

    void spawn(GoPlayer &player) noexcept;
    void attack(sfs::Scene &scene, GoPlayer &player, const sf::Vector2f &target) noexcept;
    void specialAttack(sfs::Scene &scene, GoPlayer &player, const sf::Vector2f &target) noexcept;
    void setCooldownReduction(float coeff) noexcept;
    float getCooldownReduction() const noexcept;
    void defaultCooldowns() noexcept;

protected:
    GameManager &_manager;
    const float _attCoef = 1.f;
    const float _speAttCoef = 5.f;
    float _prevAttack;
    float _prevSpecialAttack;
    float _cooldownCoef;
    std::string _spriteName;
    sfs::Sprite *_sprite;
};
} // namespace cf