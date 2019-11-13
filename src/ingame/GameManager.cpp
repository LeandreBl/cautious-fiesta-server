#include "GoUdp.hpp"
#include "GameManager.hpp"

namespace cf {
GameManager::GameManager(GameRoom &room, uint16_t port) noexcept
	: _udpPort(port)
	, _udpService()
	, _players()
	, _gameRoom(room)
{
}

void GameManager::start(sfs::Scene &scene) noexcept
{
	for (auto &&i : _gameRoom.getPlayers()) {
		auto &p = scene.addGameObject<GoPlayer>(i->name(), i->getPlayer());
		_players.push_back(&p);
	}
	scene.addGameObject<GoUdp>(*this, _udpPort);
}

std::vector<GoPlayer *> &GameManager::getGoPlayers() noexcept
{
	return _players;
}

const std::vector<PlayerConnection *> &GameManager::getConnections() noexcept
{
	return _gameRoom.getPlayers();
}
} // namespace cf