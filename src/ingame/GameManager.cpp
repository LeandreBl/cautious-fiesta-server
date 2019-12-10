#include "GoUdp.hpp"
#include "GameManager.hpp"
#include "GoMapGeneration.hpp"
#include "GoEnnemy.hpp"
#include "GoBoss.hpp"
#include "Vector.hpp"
#include "SpawnManager.hpp"

namespace cf {
GameManager::GameManager(GameRoom &room, boost::asio::io_service &localService) noexcept
	: _service(localService)
	, _players()
	, _gameRoom(room)
	, _colliderManager(nullptr)
	, _map(nullptr)
{
}

void GameManager::start(sfs::Scene &scene) noexcept
{
	for (auto &&i : _gameRoom.getPlayers()) {
		auto &p = scene.addGameObject<GoPlayer>(sf::Vector2f(1920 / 2, 1080 / 2), *this,
							i->name(), i->getPlayer());
		_players.push_back(&p);
	}
	scene.addGameObject<GoUdp>(scene, *this, _service);
	_colliderManager = &scene.addGameObject<ColliderManager>();
	_map = &scene.addGameObject<MapGenerator>(*this);
	addChild<SpawnManager>(scene, *this);
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

void GameManager::playerDeath(GoPlayer &player) noexcept
{
	for (size_t i = 0; i < _players.size(); ++i) {
		if (_players[i] == &player) {
			_players[i] = nullptr;
			return;
		}
	}
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

const GoPlayer *GameManager::getNearestPlayer(const sf::Vector2f &pos) noexcept
{
	auto &v = getGoPlayers();
	if (v.empty())
		return nullptr;
	if (v.size() == 1)
		return (v[0]);

	if (v[0] == nullptr)
		return nullptr;
	float nearestDistancePlayer = sfs::distance(v[0]->getPosition(), pos);
	auto *p = v[0];
	for (auto &&i : v) {
		if (i == nullptr)
			continue;
		float newDistance = sfs::distance(i->getPosition(), pos);
		if (newDistance < nearestDistancePlayer) {
			p = i;
			nearestDistancePlayer = newDistance;
		}
	}
	return p;
}

} // namespace cf