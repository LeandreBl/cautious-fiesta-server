#pragma once

#include <GameObject.hpp>

#include "IGoObstacle.hpp"
#include "GameManager.hpp"
#include "IGoEntity.hpp"

namespace cf {
class GoPuddle : public cf::IGoObstacle {
public:
    GoPuddle(sfs::Scene &scene, GameManager &gameManager) noexcept;
    ~GoPuddle() noexcept;

	void start(sfs::Scene &scene) noexcept;
    bool collide(const IGoEntity &entity) const noexcept;
    sf::FloatRect getGlobalBounds() noexcept;
	void onDestroy() noexcept;
    Serializer serialize() const noexcept;

private:
    std::string _assetPath;
    sfs::MultiSprite *_sprites;
    GameManager &_manager;
};
} // namespace cf
