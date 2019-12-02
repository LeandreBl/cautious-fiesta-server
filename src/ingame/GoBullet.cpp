#include "GoBullet.hpp"

namespace cf {
GoBullet::GoBullet(GameManager &manager, const sf::Vector2f &position,
		   const sf::Vector2f &vspeed) noexcept
	: IGoProjectile(manager, position, vspeed, sf::Vector2f(1, 1))
	, _spriteName("assets/fireball.png")
	, _sprite(nullptr)
{
}

void GoBullet::onDestroy() noexcept
{
	Serializer s;

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
	s << _velocity.speed.x << _velocity.speed.y;
	s << _velocity.acceleration.x << _velocity.acceleration.y;
	s << sf::Color::White;
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