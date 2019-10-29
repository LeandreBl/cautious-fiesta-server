#include <Scene.hpp>
#include <GameObject.hpp>

#include "GameRoom.hpp"
#include "Trace.hpp"

namespace cf
{
GameRoom::GameRoom(const std::string &name) noexcept : _name(name), _players()
{
}

GameRoom::~GameRoom() noexcept
{
	Serializer packet;

	packet.set(true);
	for (auto &&i : _players) {
		i->pushPacket(packet, cf::LEAVE_GAMEROOM);
		i->refreshTcp();
		i->leaveRoom();
	}
	trace(true, "#%s has terminated\n", _name.c_str());
}

void GameRoom::sendAllTcp(Serializer &packet, enum pktType_e type) const
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
			_players.erase(it);
			return;
		}
	}
}

class Timer : public sfs::GameObject
{
	public:
	void update(sfs::Scene &scene) noexcept
	{
		_prev += scene.deltaTime();
		if (_prev >= 1.f) {
			std::cout << scene.realTime() << std::endl;
			_prev = 0;
		}
		if (scene.realTime() > 5.f)
			scene.close();
	}
	float _prev = 0;
};

void GameRoom::scene() noexcept
{
	sfs::Scene scene(_name, 60);

	scene.addGameObject<Timer>();
	scene.run();
	_endCallback(*this);
}

void GameRoom::start(const std::function<void(GameRoom &)> &endCallback) noexcept
{
	_endCallback = std::move(endCallback);
	_thread = std::make_unique<std::thread>(
		std::bind(&GameRoom::scene, this));
	_thread->detach();
	trace(_thread != nullptr, "#%s started with %zu player(s).\n",
	      _name.c_str(), _players.size());
}

} // namespace cf