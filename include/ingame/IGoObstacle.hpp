#pragma once

#include "IGoEntity.hpp"

namespace cf {
class IGoObstacle : public sfs::GameObject {
      public:
	virtual bool collide(const IGoEntity &entity) const noexcept = 0;
	virtual Serializer serialize() const noexcept = 0;
};
} // namespace cf