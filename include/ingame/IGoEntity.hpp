#pragma once

#include <GameObject.hpp>

namespace cf {
class IGoEntity : public sfs::GameObject {
      public:
	virtual sf::FloatRect getHitBox() const noexcept = 0;
};
} // namespace cf
