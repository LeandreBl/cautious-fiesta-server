#include "GoEnnemy.hpp"
#include "GameManager.hpp"
#include "SpriteSheetLoader.hpp"
#include "GoBullet.hpp"
#include "Vector.hpp"

namespace cf
{

void GoEnnemy::onDestroy() noexcept
{
    Serializer s;

    s << static_cast<int32_t>(UdpPrctl::destroyType::GAMEOBJECT);
    s << getId();
    _gameManager.updateUdp(s, UdpPrctl::Type::DESTROY);
    _gameManager.getColliderManager().removeFromEnnemies(*this);
}

GoEnnemy::GoEnnemy(const sf::Vector2f &position, GameManager &manager, const std::string &ennemyName,
                   const Stats &ennemy) noexcept
    : IGoEntity(position, ennemyName, ennemy), _gameManager(manager), _ennemySprite(nullptr), _spriteSheet("assets/ENNEMY1_34x34_DIAG_SSHEET.txt"), _prevPosition(addComponent<CpnPrevPosition>()), _velocity(addComponent<sfs::Velocity>(sf::Vector2f(0, 0), sf::Vector2f(0, 0))), _prevAttack(0)

{
}

void GoEnnemy::start(sfs::Scene &scene) noexcept
{

    SpriteSheetLoader loader(_spriteSheet);
    auto path = loader.getSpritePath();
    auto v = loader.getFrames();
    auto *texture = scene.getAssetTexture(path);
    Serializer s;
    if (texture == nullptr)
    {
        std::cerr << "Unable to find " << _spriteSheet << " asset"
                  << std::endl;
        destroy();
        return;
    }
    _ennemySprite = &addComponent<sfs::MultiSprite>(*texture, v);
    s << static_cast<int32_t>(UdpPrctl::spawnType::ENNEMY) << getId() << getName()
      << _spriteSheet;
    _gameManager.getColliderManager().addToEnnemies(*this);
    _gameManager.updateUdp(s, cf::UdpPrctl::Type::SPAWN);
    s.clear();
    s << getId();
    s << getPosition().x << getPosition().y;
    _gameManager.updateUdp(s, cf::UdpPrctl::Type::POSITION);
}

void GoEnnemy::update(sfs::Scene &scene) noexcept
{
    auto t = scene.realTime();
    auto pos1 = getPosition();
    auto *player = _gameManager.getNearestPlayer(pos1);
    if (player == nullptr)
        return;
    auto pos2 = player->getPosition();
    float angle = sfs::angle(pos1, pos2) + M_PI;
    Serializer s;

    _velocity.speed.x = 20;
    s << getId();
    s << getPosition().x << getPosition().y;
    _gameManager.updateUdp(s, cf::UdpPrctl::Type::POSITION);
    if (t - _prevAttack > 1)
    {
        _prevAttack = t;
        size_t n = 1;
        float fov = M_PI / 4;
        const float delta = fov / n;
        const float off = (n % 2 == 0) ? 0 : (fov / 2);
        for (size_t i = 0; i < n; ++i)
        {
            float rangle = angle - (fov / 2) + (delta * i) + off;
            auto &b = addChild<GoBullet>(scene, _gameManager, getPosition(), rangle,
                                         800, sf::Color(255, 0, 0, 200));
            b.setAttack(getAttack());
            _gameManager.getColliderManager().addToEnnemies(b);
        }
    }
}

std::string GoEnnemy::asString() const noexcept
{
    return GameObject::asString() + " " + Stats::asString();
}

void GoEnnemy::collide(IGoEntity &entity) noexcept
{
    entity.inflictDamage(getAttack());
}

void GoEnnemy::collide(IGoObstacle &obstacle) noexcept
{
    Serializer s;

    s << getId();
    s << getPosition().x << getPosition().y;
    _gameManager.updateUdp(s, UdpPrctl::Type::POSITION);
}

void GoEnnemy::goToPrevPosition() noexcept
{
    setPosition(_prevPosition.getPrevPosition());
}

void GoEnnemy::updateUdpVelocity() noexcept
{
    Serializer s;
    s << getId() << _velocity.speed.x << _velocity.speed.y << _velocity.acceleration.x
      << _velocity.acceleration.y;
    _gameManager.updateUdp(s, UdpPrctl::Type::VELOCITY);
}

sfs::Velocity &GoEnnemy::getVelocity() noexcept
{
    return _velocity;
}

sf::FloatRect GoEnnemy::getHitBox() const noexcept
{
    /* TODO with sprite size */
    return _ennemySprite->getGlobalBounds();
}

} // namespace cf