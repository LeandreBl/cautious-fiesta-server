#include "GameRoom.hpp"
#include "GameManager.hpp"

namespace cf {
void GameRoom::scene() noexcept
{
	sfs::Scene scene(_name, 60);

	_scene = &scene;
	scene.addGameObject<GameManager>(*this);
	scene.run();
	scene.clear();
	_isRunning = false;
	_scene = nullptr;
	_endCallback(*this);
}
} // namespace cf