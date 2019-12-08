#include "GoBullet.hpp"
#include "CpnTimer.hpp"

namespace cf {
GoBullet::GoBullet(GameManager &manager, const sf::Vector2f &position, float angle, float speed,
		   const sf::Color &color) noexcept
	: IGoProjectile(manager, position, angle, speed)
	, _angle(angle)
	, _speed(speed)
	, _spriteName("assets/Projectiles.png")
	, _sprite(nullptr)
	, _color(color)
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
}

void GoBullet::start(sfs::Scene &scene) noexcept
{
	auto *texture = scene.getAssetTexture(_spriteName);

	if (texture == nullptr) {
		std::cerr << "Can't load " << _spriteName << std::endl;
		destroy();
		return;
	}
	_sprite = &addComponent<sfs::Sprite>(*texture);
	Serializer s;

	s << static_cast<int32_t>(UdpPrctl::spawnType::PROJECTILE);
	s << static_cast<uint64_t>(getId());
	s << getPosition().x << getPosition().y;
	s << _angle << _speed;
	s << _color;
	s << _spriteName;
	_manager.getColliderManager().addToAllies(*this);
	_manager.updateUdp(s, UdpPrctl::Type::SPAWN);
}

void GoBullet::collide(IGoEntity &entity) noexcept
{
	entity.inflictDamage(getAttack());
	destroy();
}

sf::FloatRect GoBullet::getHitBox() const noexcept
{
	return _sprite->getGlobalBounds();
}
} // namespace cf