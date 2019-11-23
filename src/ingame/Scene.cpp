#include "GameRoom.hpp"
#include "GameManager.hpp"

namespace cf {
void GameRoom::scene() noexcept
{
	sfs::Scene scene(_name, 60);

	scene.addGameObject<GameManager>(*this);
	scene.run();
	_endCallback(*this);
}
} // namespace cf