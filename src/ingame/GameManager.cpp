#include "GoUdp.hpp"
#include "GameManager.hpp"
#include "GoMapGeneration.hpp"
#include "GoEnnemy.hpp"
#include "GoBoss.hpp"
#include "Vector.hpp"

namespace cf
{
GameManager::GameManager(GameRoom &room, boost::asio::io_service &localService) noexcept
	: _service(localService), _players(), _gameRoom(room), _colliderManager(nullptr), _map(nullptr)
{
}

void GameManager::start(sfs::Scene &scene) noexcept
{
	struct Stats ennemyStats;
	struct Stats bossStats;
	ennemyStats.setArmor(20);
	ennemyStats.setAttack(5);
	ennemyStats.setAttackSpeed(1);
	ennemyStats.setLife(50);
	ennemyStats.setSpeed(20);
	bossStats.setArmor(50);
	bossStats.setAttack(20);
	bossStats.setAttackSpeed(2);
	bossStats.setLife(200);
	bossStats.setSpeed(20);
	for (auto &&i : _gameRoom.getPlayers())
	{
		auto &p = scene.addGameObject<GoPlayer>(sf::Vector2f(1920 / 2, 1080 / 2), *this,
												i->name(), i->getPlayer());
		_players.push_back(&p);
	}
	scene.addGameObject<GoUdp>(scene, *this, _service);
	_colliderManager = &scene.addGameObject<ColliderManager>();
	_map = &scene.addGameObject<MapGenerator>(*this);
	scene.addGameObject<GoEnnemy>(sf::Vector2f(1920 / 4, 1080 / 4), *this, "NoeuNoeil", ennemyStats);
	//scene.addGameObject<GoEnnemy>(sf::Vector2f(1920 / 1.5, 1080 / 1.5), *this, "NoeilNoeil", ennemyStats);
	//scene.addGameObject<GoBoss>(sf::Vector2f(1920 / 5, 1080 / 5), *this, "Cervelas", bossStats);
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
	for (auto &&c : getConnections())
	{
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

	float nearestDistancePlayer = sfs::distance(v[0]->getPosition(), pos);
	auto *p = v[0];
	for (auto &&i : v)
	{
		float newDistance = sfs::distance(i->getPosition(), pos);
		if (newDistance < nearestDistancePlayer)
		{
			p = i;
			nearestDistancePlayer = newDistance;
		}
	}
	return p;
}

} // namespace cf