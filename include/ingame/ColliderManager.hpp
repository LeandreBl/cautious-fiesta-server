#pragma once

#include <queue>
#include <vector>

#include <GameObject.hpp>

#include "IGoEntity.hpp"
#include "IGoObstacle.hpp"

namespace cf {
template <typename T> struct container {
	std::queue<T *> toAdd;
	std::vector<T *> container;
	std::queue<T *> toRemove;
};
class ColliderManager : public sfs::GameObject {
      public:
	void removeFromAllies(IGoObstacle &obstacle) noexcept;
	void removeFromAllies(IGoEntity &entity) noexcept;
	void removeFromEnnemies(IGoObstacle &obstacle) noexcept;
	void removeFromEnnemies(IGoEntity &entity) noexcept;
	void addToAllies(IGoObstacle &obstacle) noexcept;
	void addToAllies(IGoEntity &entity) noexcept;
	void addToEnnemies(IGoObstacle &obstacle) noexcept;
	void addToEnnemies(IGoEntity &entity) noexcept;

	void updateToDelete() noexcept;
	void updateToAdd() noexcept;

	void update(sfs::Scene &scene) noexcept;

      private:
	struct container<IGoEntity> _ennemyEntities;
	struct container<IGoEntity> _allyEntities;
	struct container<IGoObstacle> _ennemyObstacles;
	struct container<IGoObstacle> _allyObstacles;
};
} // namespace cf