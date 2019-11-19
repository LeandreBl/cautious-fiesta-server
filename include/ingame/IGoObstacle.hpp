#pragma once

#include "IGoEntity.hpp"

namespace cf {
class IGoObstacle : public sfs::GameObject {
      public:
	virtual bool collide(const IGoEntity &entity) const noexcept = 0;
};
} // namespace cf