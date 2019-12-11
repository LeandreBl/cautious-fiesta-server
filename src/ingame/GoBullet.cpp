#include "GoBullet.hpp"
#include "CpnTimer.hpp"
#include "SpriteSheetLoader.hpp"

namespace cf
{
GoBullet::GoBullet(GameManager &manager, const sf::Vector2f &position, float angle, float speed,
				   const sf::Color &color) noexcept
	: IGoProjectile(manager, position, angle, speed), _angle(angle), _speed(speed), _spriteSheet("assets/SpriteSheets/Projectiles.txt"), _sprites(nullptr), _color(color)
{
	addComponent<CpnTimer>(2);
}

void GoBullet::onDestroy() noexcept
{
	Serializer s;

	s << static_cast<int32_t>(UdpPrctl::destroyType::GAMEOBJECT);
	s << getId();
	_manager.updateUdp(s, UdpPrctl::Type::DESTROY);
	_manager.getColliderManager().removeFromAllies(*this);
	_manager.getColliderManager().removeFromEnnemies(*this);
}

void GoBullet::start(sfs::Scene &scene) noexcept
{
	SpriteSheetLoader loader(_spriteSheet);
	auto *texture = scene.getAssetTexture(loader.getSpritePath());

	if (texture == nullptr)
	{
		std::cerr << "Can't load " << _spriteSheet << std::endl;
		destroy();
		return;
	}
	_sprites = &addComponent<sfs::MultiSprite>(*texture, loader.getFrames());
	Serializer s;

	s << static_cast<int32_t>(UdpPrctl::spawnType::PROJECTILE);
	s << static_cast<uint64_t>(getId());
	s << getPosition().x << getPosition().y;
	s << _angle << _speed;
	s << _color;
	s << _spriteSheet;
	_manager.updateUdp(s, UdpPrctl::Type::SPAWN);
}

void GoBullet::collide(IGoEntity &entity) noexcept
{
	entity.inflictDamage(getAttack());
	if (entity.getEntityType() == EntityType::PLAYER) {
		Serializer s;

		s << static_cast<int32_t>(UdpPrctl::stateType::SETLIFE);
		s << static_cast<uint64_t>(entity.getId());
		s << entity.getLife();
		_manager.updateUdp(s, UdpPrctl::Type::STATE);
	}
	if (entity.getLife() <= 1) {
		if (entity.getEntityType() == EntityType::ENNEMY) {
			auto player = static_cast<IGoEntity*>(this->parent());
			if (player->getEntityType() == EntityType::PLAYER) {
				static_cast<GoPlayer *>(player)->confirmKill();
			}
		}
		entity.destroy();
	}
	destroy();
}

void GoBullet::collide(IGoObstacle &obstacle) noexcept
{
	destroy();
}

sf::FloatRect GoBullet::getHitBox() const noexcept
{
	sf::FloatRect rect(0, 0, 0, 0);
	if (_sprites != nullptr)
	{
		rect = _sprites->getGlobalBounds();
	}
	rect.left = getPosition().x;
	rect.top = getPosition().y;
	return rect;
}
} // namespace cf