#pragma once

#include "IGoProjectile.hpp"

namespace cf {
class GoBullet : public IGoProjectile {
      public:
	GoBullet(GameManager &manager, const sf::Vector2f &position, float angle, float speed,
		 const sf::Color &color = sf::Color::White) noexcept;
	void onDestroy() noexcept;
	void start(sfs::Scene &scene) noexcept;
	void collide(IGoEntity &entity) noexcept;
	sf::FloatRect getHitBox() const noexcept;

      protected:
	float _angle;
	float _speed;
	std::string _spriteName;
	sfs::Sprite *_sprite;
	sf::Color _color;
};
} // namespace cf