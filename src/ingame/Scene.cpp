#include "GameRoom.hpp"
#include "GameManager.hpp"

namespace cf {
void GameRoom::scene(uint16_t port) noexcept
{
	sfs::Scene scene(_name, 60);

	scene.addGameObject<GameManager>(*this, port);
	scene.run();
	_endCallback(*this);
}
} // namespace cf