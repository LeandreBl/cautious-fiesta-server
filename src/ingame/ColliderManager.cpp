#include "ColliderManager.hpp"

namespace cf {

template <typename T> static void removeIn(std::vector<T *> &v, T &o)
{
	for (auto it = v.begin(); it != v.end(); ++it) {
		if ((*it) == &o) {
			v.erase(it);
			return;
		}
	}
}

void ColliderManager::removeFromAllies(IGoObstacle &go) noexcept
{
	removeIn<typeof(go)>(_allyObstacles, go);
}

void ColliderManager::removeFromAllies(IGoEntity &go) noexcept
{
	removeIn<typeof(go)>(_allyEntities, go);
}

void ColliderManager::removeFromEnnemies(IGoObstacle &go) noexcept
{
	removeIn<typeof(go)>(_ennemyObstacles, go);
}

void ColliderManager::removeFromEnnemies(IGoEntity &go) noexcept
{
	removeIn<typeof(go)>(_ennemyEntities, go);
}

void ColliderManager::addToAllies(IGoObstacle &obstacle) noexcept
{
	_allyObstacles.push_back(&obstacle);
}

void ColliderManager::addToAllies(IGoEntity &entity) noexcept
{
	_allyEntities.push_back(&entity);
}

void ColliderManager::addToEnnemies(IGoObstacle &obstacle) noexcept
{
	_ennemyObstacles.push_back(&obstacle);
}

void ColliderManager::addToEnnemies(IGoEntity &entity) noexcept
{
	_ennemyEntities.push_back(&entity);
}

void ColliderManager::update(sfs::Scene &scene) noexcept
{
	for (auto &&e : _allyEntities) {
		for (auto &&o : _ennemyObstacles)
			if (o->collide(*e) == true)
				e->goToPrevPosition();
		for (auto &&eo : _ennemyEntities) {
			if (e->getHitBox().intersects(eo->getHitBox())) {
				e->collide(*eo);
				eo->collide(*e);
			}
		}
	}
	for (auto &&e : _ennemyEntities)
		for (auto &&o : _allyObstacles)
			if (o->collide(*e) == true)
				e->goToPrevPosition();
}
} // namespace cf