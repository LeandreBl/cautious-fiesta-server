#pragma once

#include <Scene.hpp>
#include <boost/asio.hpp>

#include "IGoWeapon.hpp"
#include "IGoEntity.hpp"
#include "CpnPrevPosition.hpp"
#include "Stats.hpp"

namespace cf
{
using boost::asio::ip::udp;
class GameManager;

class GoBoss : public IGoEntity
{
public:
    void onDestroy() noexcept;
    GoBoss(const sf::Vector2f &position, GameManager &manager, const std::string &bossName,
           const Stats &boss) noexcept;
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
    sfs::MultiSprite *_bossSprite;
    sfs::Velocity &_velocity;
    std::string _spriteSheet;
    float _prevAttack;
};
} // namespace cf
