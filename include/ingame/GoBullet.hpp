#pragma once

#include "IGoProjectile.hpp"

namespace cf {
class GoBullet : public IGoProjectile {
      public:
	GoBullet(GameManager &manager, const sf::Vector2f &position,
		 const sf::Vector2f &vspeed) noexcept;
	void onDestroy() noexcept;
	void start(sfs::Scene &scene) noexcept;
	void collide(IGoEntity &entity) noexcept;
	sf::FloatRect getHitBox() const noexcept;

      protected:
	std::string _spriteName;
	sfs::Sprite *_sprite;
};
} // namespace cf