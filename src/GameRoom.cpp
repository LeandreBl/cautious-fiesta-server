#include "GameRoom.hpp"

namespace cf
{
GameRoom::GameRoom(const std::string &name) noexcept : _name(name)
{
}

void GameRoom::sendAllTcp(Serializer &packet, enum pktType_e type) const noexcept
{
	for (auto &&i : _players) {
		i->pushPacket(packet, type);
	}
}

const std::string &GameRoom::getName() const noexcept
{
	return _name;
}

const std::vector<PlayerConnection *> &GameRoom::getPlayers() const noexcept
{
	return _players;
}

void GameRoom::join(PlayerConnection &handle) noexcept
{
	for (auto &&i : _players)
		if (i->getId() == handle.getId())
			return;
	_players.push_back(&handle);
	handle.room(*this);
}

void GameRoom::leave(PlayerConnection &handle) noexcept
{
	_players.erase(std::remove(_players.begin(), _players.end(), &handle));
}

} // namespace cf