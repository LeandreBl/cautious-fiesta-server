#pragma once

#include <boost/asio.hpp>
#include <GameObject.hpp>

#include "GameRoom.hpp"
#include "GoPlayer.hpp"


namespace cf {
template <typename T> T &operator<<(T &os, const sf::Vector2f &v)
{
	os << '(' << v.x << ", " << v.y << ')';
	return os;
}

template <typename T> T &operator<<(T &os, const sf::Vector3f &v) noexcept
{
	os << '(' << v.x << ", " << v.y << ", " << v.z << ')';
	return os;
}

template <typename T, typename B> T &operator<<(T &os, const sf::Rect<B> &rect) noexcept
{
	os << "[(" << rect.top << ", " << rect.left << "), (" << rect.width << ", " << rect.height
	   << ")]";
	return os;
}
class GameManager : public sfs::GameObject {
      public:
	GameManager(GameRoom &room, uint16_t port) noexcept;
	void start(sfs::Scene &scene) noexcept;
	std::vector<GoPlayer *> &getGoPlayers() noexcept;
	const std::vector<PlayerConnection *> &getConnections() noexcept;

      private:
	uint16_t _udpPort;
	boost::asio::io_service _udpService;
	std::vector<GoPlayer *> _players;
	GameRoom &_gameRoom;
};
} // namespace cf