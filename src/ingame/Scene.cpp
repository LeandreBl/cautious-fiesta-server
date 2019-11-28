#include "GameRoom.hpp"
#include "GameManager.hpp"

namespace cf {
void GameRoom::scene() noexcept
{
	sfs::Scene scene(_name, 60);

	_scene = &scene;
	scene.addGameObject<GameManager>(*this, _service);
	scene.run();
	scene.clear();
	_service.stop();
	for (auto &&i : _players) {
		i->closeUdp();
	}
	_isRunning = false;
	_scene = nullptr;
	_endCallback(*this);
}
} // namespace cf