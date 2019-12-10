#include "GoMapGeneration.hpp"

namespace cf {

MapGenerator::MapGenerator(GameManager &gameManager, int scale, int density) noexcept
	: _mapSize()
	, _manager(gameManager)
{
	std::random_device rd;
	_gen = std::mt19937(rd());
	// _randMapSize = std::uniform_int_distribution<>(7, 10);
	// _mapSize = _randMapSize(_gen) * 2;
	_mapSize = 20;
	_randObjPosX = std::uniform_int_distribution<>(1, 37);
	_randObjPosY = std::uniform_int_distribution<>(1, 19);
}

void MapGenerator::start(sfs::Scene &scene) noexcept
{
	for (int i = 0; i < 20 + 20 % 2; i += 1) {
		auto &goUp = addChild<GoWallHorizontal>(scene, scene, _manager);
		goUp.setPosition(
			i * goUp.getGlobalBounds().width + goUp.getGlobalBounds().width / 2, 0);

		auto &goDown = addChild<GoWallHorizontal>(scene, scene, _manager);
		goDown.setPosition(i * goDown.getGlobalBounds().width,
				   (10 * goDown.getGlobalBounds().width)
					   + (11 % 2 * goDown.getGlobalBounds().width) / 2);

		auto &goLeft = addChild<GoWallVertical>(scene, scene, _manager);
		goLeft.setPosition(0, i * goLeft.getGlobalBounds().height);

		auto &goRight = addChild<GoWallVertical>(scene, scene, _manager);
		goRight.setPosition(18 * goRight.getGlobalBounds().height
					    + (19 % 2 * goRight.getGlobalBounds().height),
				    i * goRight.getGlobalBounds().height
					    + goRight.getGlobalBounds().height / 2);

		if (i % 3 == 0) {
			auto &goB = addChild<GoBarrel>(scene, scene, _manager);
			goB.setPosition(_randObjPosX(_gen) * goB.getGlobalBounds().width,
					_randObjPosY(_gen) * goB.getGlobalBounds().height);
		}

		if (i % 4 == 0) {
			auto &goP = addChild<GoPuddle>(scene, scene, _manager);
			goP.setPosition(_randObjPosX(_gen) * goP.getGlobalBounds().width,
					_randObjPosY(_gen) * goP.getGlobalBounds().height);
		}
	}
}
} // namespace cf