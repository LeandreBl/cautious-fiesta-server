#pragma once

#include <GameObject.hpp>

#include "IGoEntity.hpp"
#include "IGoObstacle.hpp"

namespace cf {
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
	void update(sfs::Scene &scene) noexcept;

      private:
	std::vector<IGoEntity *> _ennemyEntities;
	std::vector<IGoEntity *> _allyEntities;
	std::vector<IGoObstacle *> _ennemyObstacles;
	std::vector<IGoObstacle *> _allyObstacles;
};
} // namespace cf