#pragma once

#include <Scene.hpp>
#include <IComponent.hpp>
#include <boost/asio.hpp>

#include "IGoWeapon.hpp"
#include "IGoEntity.hpp"
#include "CpnPrevPosition.hpp"
#include "Stats.hpp"

namespace cf
{
using boost::asio::ip::udp;
class GameManager;

class Wandering : public sfs::IComponent
{
public:
    Wandering(float delta) noexcept
        : _delta(delta)
    {
    }
    void start(sfs::Scene &scene, sfs::GameObject &go) noexcept
    {
        _previous = scene.time();
    }
    void update(sfs::Scene &scene, sfs::GameObject &go) noexcept
    {
        float dt = scene.time();

        if (dt - _previous > _delta)
        {
            _previous = dt;
            auto v = go.getComponents<sfs::Velocity>();
            for (auto &&c : v)
            {
                c->speed.x *= -1;
            }
        }
    }
    float _delta;
    float _previous;
};

class GoEnnemy : public IGoEntity
{
public:
    void onDestroy() noexcept;
    GoEnnemy(const sf::Vector2f &position, GameManager &manager, const std::string &ennemyName,
             const Stats &ennemy) noexcept;
    void start(sfs::Scene &scene) noexcept;
    void update(sfs::Scene &scene) noexcept;
    void attack(sfs::Scene &scene) noexcept;
    std::string asString() const noexcept;
    void collide(IGoEntity &entity) noexcept;
    void collide(IGoObstacle &obstacle) noexcept;
    void goToPrevPosition() noexcept;
    void updateUdpVelocity() noexcept;
    sfs::Velocity &getVelocity() noexcept;
    sf::FloatRect getHitBox() const noexcept;
    EntityType getEntityType() noexcept { return EntityType::ENNEMY; };

private:
    GameManager &_gameManager;
    CpnPrevPosition &_prevPosition;
    sfs::MultiSprite *_ennemySprite;
    sfs::Velocity &_velocity;
    std::string _spriteSheet;
    float _prevAttack;
};
} // namespace cf
