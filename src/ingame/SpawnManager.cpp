#include "SpawnManager.hpp"
#include "random"

namespace cf
{
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
	    _ennemiStats.setAttackSpeed(1);
	    _ennemiStats.setLife(50);
	    _ennemiStats.setSpeed(20);
    }

    void SpawnManager::update(sfs::Scene &scene) noexcept
    {
        auto time = scene.time();
        if (time - _difficultytime >= 7) { //diviser par le nb player
            _difficulty += 1;
            _difficultytime = time;
        }
        if (time - _time >= 3) {
            for (int i = 0; i != _difficulty; i += 1) {
	            addChild<GoEnnemy>(scene, sf::Vector2f((float)(rand() % 1921), (float)(rand() % 1081)), _gameManager, std::to_string(_ennemiIdName), _ennemiStats);
                _ennemiIdName += 1;
            }
            _time = time;
        }
    }   
}


