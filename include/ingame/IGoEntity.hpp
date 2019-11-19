#pragma once

#include <GameObject.hpp>

namespace cf {
class IGoEntity : public sfs::GameObject {
      public:
	virtual void goToPrevPosition() noexcept = 0;
	virtual sf::FloatRect getHitBox() const noexcept = 0;
	virtual void collide(IGoEntity &entity) noexcept = 0;
};
} // namespace cf
