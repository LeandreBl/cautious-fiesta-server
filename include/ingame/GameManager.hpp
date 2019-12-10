#pragma once

#include <boost/asio.hpp>
#include <GameObject.hpp>

#include "GameRoom.hpp"
#include "GoPlayer.hpp"
#include "ColliderManager.hpp"

namespace cf
{
class MapGenerator;

template <typename T>
T &operator<<(T &os, const sf::Vector2f &v)
{
	os << '(' << v.x << ", " << v.y << ')';
	return os;
}

template <typename T>
T &operator<<(T &os, const sf::Vector3f &v) noexcept
{
	os << '(' << v.x << ", " << v.y << ", " << v.z << ')';
	return os;
}

template <typename T, typename B>
T &operator<<(T &os, const sf::Rect<B> &rect) noexcept
{
	os << "[(" << rect.top << ", " << rect.left << "), (" << rect.width << ", " << rect.height
	   << ")]";
	return os;
}

class GameManager : public sfs::GameObject
{
public:
	GameManager(GameRoom &room, boost::asio::io_service &localService) noexcept;

	void start(sfs::Scene &scene) noexcept;

	ColliderManager &getColliderManager() noexcept;
	std::vector<GoPlayer *> &getGoPlayers() noexcept;
	const std::vector<PlayerConnection *> &getConnections() noexcept;
	void updateUdp(const Serializer &s, UdpPrctl::Type type) noexcept;
	const std::string &getName() const noexcept;
	const GoPlayer *getNearestPlayer(const sf::Vector2f &pos) noexcept;

private:
	boost::asio::io_service &_service;
	std::vector<GoPlayer *> _players;
	GameRoom &_gameRoom;
	ColliderManager *_colliderManager;
	MapGenerator *_map;
};
} // namespace cf