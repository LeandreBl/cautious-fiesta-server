#pragma once

#include <vector>
#include <random>
#include <GameObject.hpp>

#include "IGoObstacle.hpp"
#include "GameManager.hpp"
#include "GoWallHorizontal.hpp"
#include "GoWallVertical.hpp"
#include "GoBarrel.hpp"
#include "GoPuddle.hpp"

namespace cf {

class MapGenerator : public sfs::GameObject {
public:
    MapGenerator(GameManager &gameManager, int scale = 0, int density = 0) noexcept;

	void start(sfs::Scene &scene) noexcept;

private:
    int _mapSize;
    GameManager &_manager;

    std::mt19937 _gen;
    std::uniform_int_distribution<> _randMapSize;
    std::uniform_int_distribution<> _randObjPos;
};
} // namespace cf