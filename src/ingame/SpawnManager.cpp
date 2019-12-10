#include "SpawnManager.hpp"
#include "random"

namespace cf {
SpawnManager::SpawnManager(GameManager &manager) noexcept
	: _gameManager(manager)
{
}

void SpawnManager::start(sfs::Scene &scene) noexcept
{
	srand(time(nullptr));
	_time = scene.time();
	_difficultytime = _time;
	_ennemiStats.setArmor(20);
	_ennemiStats.setAttack(5);
	_ennemiStats.setAttackSpeed((float)(rand() % 10) / 10);
	_ennemiStats.setLife(50);
	_ennemiStats.setSpeed(20);
}

void SpawnManager::ennemisStatsUp() noexcept
{
	struct Stats::stats toAdd;

	toAdd.life = 2;
	toAdd.speed = 2;
	toAdd.attack = 2;
	toAdd.attackSpeed = 0.2;
	toAdd.armor = 2;
	_ennemiStats.levelUp(toAdd);
}

void SpawnManager::update(sfs::Scene &scene) noexcept
{
	auto time = scene.time();
	if (time - _difficultytime >= 7 / _gameManager.getGoPlayers().size()) {
		_difficulty += 1;
		if (_difficulty % 2 == 0)
			ennemisStatsUp();
		_difficultytime = time;
	}
	if (time - _time >= 3) {
		for (int i = 0; i != _difficulty; i += 1) {
			if ((rand() % 101) >= 96)
				addChild<GoBoss>(scene,
						 sf::Vector2f((float)(rand() % 1921),
							      (float)(rand() % 1081)),
						 _gameManager, std::to_string(_ennemiIdName),
						 _ennemiStats);
			else
				addChild<GoEnnemy>(scene,
						   sf::Vector2f((float)(rand() % 1921),
								(float)(rand() % 1081)),
						   _gameManager, std::to_string(_ennemiIdName),
						   _ennemiStats);
			_ennemiIdName += 1;
		}
		_time = time;
	}
}
} // namespace cf
