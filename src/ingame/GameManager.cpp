#include "GoUdp.hpp"
#include "GameManager.hpp"

namespace cf {
GameManager::GameManager(GameRoom &room, boost::asio::io_service &localService) noexcept
	: _service(localService)
	, _players()
	, _gameRoom(room)
	, _colliderManager(nullptr)
{
}

void GameManager::start(sfs::Scene &scene) noexcept
{
	for (auto &&i : _gameRoom.getPlayers()) {
		auto &p = scene.addGameObject<GoPlayer>(sf::Vector2f(0, 0), *this, i->name(),
							i->getPlayer());
		_players.push_back(&p);
	}
	scene.addGameObject<GoUdp>(*this, _service);
	_colliderManager = &scene.addGameObject<ColliderManager>();
}

ColliderManager &GameManager::getColliderManager() noexcept
{
	return *_colliderManager;
}

std::vector<GoPlayer *> &GameManager::getGoPlayers() noexcept
{
	return _players;
}

const std::vector<PlayerConnection *> &GameManager::getConnections() noexcept
{
	return _gameRoom.getPlayers();
}

void GameManager::updateUdp(const Serializer &s, UdpPrctl::Type type) noexcept
{
	for (auto &&c : getConnections()) {
		c->pushUdpPacket(s, type);
	}
}

const std::string &GameManager::getName() const noexcept
{
	return _gameRoom.getName();
}

} // namespace cf