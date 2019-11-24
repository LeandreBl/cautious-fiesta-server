#include <Scene.hpp>
#include <GameObject.hpp>

#include "GameRoom.hpp"
#include <Trace.hpp>

namespace cf {
GameRoom::GameRoom(const std::string &name) noexcept
	: _name(name)
	, _players()
	, _isRunning(false)
{
}

GameRoom::~GameRoom() noexcept
{
	Serializer packet;

	packet.set(true);
	for (auto &&i : _players) {
		if (i->isLogged()) {
			i->pushPacket(packet, TcpPrctl::Type::LEAVE_GAMEROOM);
			i->refreshTcp();
			i->leaveRoom();
		}
	}
	trace(true, "#%s has terminated\n", _name.c_str());
}

void GameRoom::sendAllTcp(Serializer &packet, TcpPrctl::Type type) const noexcept
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
	for (auto it = _players.begin(); it != _players.end(); ++it) {
		if ((*it)->getId() == handle.getId()) {
			(*it)->ready(false);
			_players.erase(it);
			return;
		}
	}
}

void GameRoom::start(const std::function<void(GameRoom &)> &endCallback) noexcept
{
	Serializer packet;

	_isRunning = true;
	_endCallback = std::move(endCallback);
	_thread = std::make_unique<std::thread>(std::bind(&GameRoom::scene, this));
	_thread->detach();
	trace(_thread != nullptr, "#%s started with %zu player(s).\n", _name.c_str(),
	      _players.size());
}

void GameRoom::stop() noexcept
{
	if (_scene)
		_scene->close();
}

bool GameRoom::isRunning() const noexcept
{
	return _isRunning;
}
} // namespace cf