#include "ColliderManager.hpp"

namespace cf
{

template <typename T>
static void removeIn(std::vector<T *> &v, T *o)
{
	for (auto it = v.begin(); it != v.end(); ++it)
	{
		if ((*it) == o)
		{
			v.erase(it);
			return;
		}
	}
}

void ColliderManager::removeFromAllies(IGoObstacle &go) noexcept
{
	_allyObstacles.toRemove.push(&go);
}

void ColliderManager::removeFromAllies(IGoEntity &go) noexcept
{
	_allyEntities.toRemove.push(&go);
}

void ColliderManager::removeFromEnnemies(IGoObstacle &go) noexcept
{
	_ennemyObstacles.toRemove.push(&go);
}

void ColliderManager::removeFromEnnemies(IGoEntity &go) noexcept
{
	_ennemyEntities.toRemove.push(&go);
}

void ColliderManager::addToAllies(IGoObstacle &obstacle) noexcept
{
	_allyObstacles.toAdd.push(&obstacle);
}

void ColliderManager::addToAllies(IGoEntity &entity) noexcept
{
	_allyEntities.toAdd.push(&entity);
}

void ColliderManager::addToEnnemies(IGoObstacle &obstacle) noexcept
{
	_ennemyObstacles.toAdd.push(&obstacle);
}

void ColliderManager::addToEnnemies(IGoEntity &entity) noexcept
{
	_ennemyEntities.toAdd.push(&entity);
}

template <typename T>
static void addV(struct container<T> &container) noexcept
{
	while (!container.toAdd.empty())
	{
		auto &go = container.toAdd.front();
		container.container.push_back(go);
		container.toAdd.pop();
	}
}

template <typename T>
static void removeV(struct container<T> &container) noexcept
{
	while (!container.toRemove.empty())
	{
		auto &go = container.toRemove.front();
		removeIn<T>(container.container, go);
		container.toRemove.pop();
	}
}

void ColliderManager::updateToDelete() noexcept
{
	removeV(_ennemyEntities);
	removeV(_allyEntities);
	removeV(_ennemyObstacles);
	removeV(_allyObstacles);
}

void ColliderManager::updateToAdd() noexcept
{
	addV(_ennemyEntities);
	addV(_allyEntities);
	addV(_ennemyObstacles);
	addV(_allyObstacles);
}

void ColliderManager::update(sfs::Scene &) noexcept
{
	updateToDelete();
	updateToAdd();
	for (auto &&e : _allyEntities.container)
	{
		for (auto &&o : _ennemyObstacles.container)
			if (o->collide(*e) == true)
			{
				e->goToPrevPosition();
				e->collide(*o);
			}
		for (auto &&eo : _ennemyEntities.container)
		{
			if (e->getHitBox().intersects(eo->getHitBox()))
			{
				e->collide(*eo);
				eo->collide(*e);
			}
		}
	}
	for (auto &&e : _ennemyEntities.container)
		for (auto &&o : _allyObstacles.container)
			if (o->collide(*e) == true)
			{
				e->goToPrevPosition();
				e->collide(*o);
			}
}
} // namespace cf