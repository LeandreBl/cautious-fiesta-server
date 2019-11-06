#include <Scene.hpp>
#include <GameObject.hpp>

#include "GameRoom.hpp"
#include <Trace.hpp>

namespace cf
{
GameRoom::GameRoom(const std::string &name) noexcept
    : _name(name), _players(), _isRunning(false)
{
}

GameRoom::~GameRoom() noexcept
{
	Serializer packet;

	packet.set(true);
	for (auto &&i : _players) {
		i->pushPacket(packet, TcpPrctl::Type::LEAVE_GAMEROOM);
		i->refreshTcp();
		i->leaveRoom();
	}
	trace(true, "#%s has terminated\n", _name.c_str());
}

void GameRoom::sendAllTcp(Serializer &packet, TcpPrctl::Type type) const
	noexcept
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

class Timer : public sfs::GameObject
{
      public:
	Timer(PlayerConnection &p) noexcept : _player(p)
	{
	}
	void update(sfs::Scene &scene) noexcept
	{
		_prev += scene.deltaTime();
		if (_prev >= 1.f) {
			Serializer a;
			float t = scene.realTime();
			a.set(static_cast<uint32_t>(UdpPrctl::timeType::REALTIME));
			a.set(t);
			_player.pushUdp(a, UdpPrctl::Type::TIME);
			_player.refreshUdp();
			_prev = 0;
		}
		if (scene.realTime() > 5.f)
			scene.close();
	}
	PlayerConnection &_player;
	float _prev = 0;
};

void GameRoom::scene() noexcept
{
	sfs::Scene scene(_name, 60);

	scene.addGameObject<Timer>(*_players.front());
	scene.run();
	_endCallback(*this);
}

void GameRoom::start(const std::function<void(GameRoom &)> &endCallback,
		     uint16_t port) noexcept
{
	Serializer packet;

	_isRunning = true;
	packet.set(port);
	for (auto &&i : _players) {
		i->pushPacket(packet, TcpPrctl::Type::GAME_STARTED);
		i->refreshTcp();
		i->setUdpPort(port);
	}
	_endCallback = std::move(endCallback);
	_thread = std::make_unique<std::thread>(
		std::bind(&GameRoom::scene, this));
	_thread->detach();
	trace(_thread != nullptr, "#%s started with %zu player(s).\n",
	      _name.c_str(), _players.size());
}

bool GameRoom::isRunning() const noexcept
{
	return _isRunning;
}
} // namespace cf