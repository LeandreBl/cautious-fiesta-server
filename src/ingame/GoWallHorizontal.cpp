#include "GoWallHorizontal.hpp"
#include "SpriteSheetLoader.hpp"

namespace cf {

GoWallHorizontal::GoWallHorizontal(sfs::Scene &scene, GameManager &gameManager) noexcept
	: _assetPath("assets/WALL_HORIZONTAL_100x50.txt")
	, _sprites(nullptr)
	, _manager(gameManager)
{
	SpriteSheetLoader loader(_assetPath);

	auto v = loader.getFrames();
	auto name = loader.getSpritePath();
	auto *texture = scene.getAssetTexture(name);

	if (texture == nullptr) {
		std::cerr << "Error: " << name << " does not exist" << std::endl;
		destroy();
		return;
	}
	_sprites = &addComponent<sfs::MultiSprite>(*texture, v, 0);
}

GoWallHorizontal::~GoWallHorizontal() noexcept
{
}

void GoWallHorizontal::start(sfs::Scene &scene) noexcept
{
	Serializer s;

	s << static_cast<int32_t>(UdpPrctl::spawnType::OBSTACLE);
	s << static_cast<uint64_t>(getId());
	s << getPosition().x << getPosition().y;
	s << _assetPath;
	_manager.updateUdp(s, UdpPrctl::Type::SPAWN);

	_manager.getColliderManager().addToAllies(*this);
	_manager.getColliderManager().addToEnnemies(*this);
}

sf::FloatRect GoWallHorizontal::getGlobalBounds() noexcept
{
	return _sprites->getGlobalBounds();
}

void GoWallHorizontal::onDestroy() noexcept
{
	_manager.getColliderManager().removeFromAllies(*this);
	_manager.getColliderManager().removeFromEnnemies(*this);
}

bool GoWallHorizontal::collide(const IGoEntity &entity) const noexcept
{
	return entity.getHitBox().intersects(_sprites->getGlobalBounds());
}

Serializer GoWallHorizontal::serialize() const noexcept
{
}
} // namespace cf