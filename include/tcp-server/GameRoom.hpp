#pragma once

#include <thread>
#include <memory>

#include "PlayerConnection.hpp"

namespace cf {
class GameRoom {
      public:
	GameRoom(const std::string &name) noexcept;
	~GameRoom() noexcept;
	void sendAllTcp(Serializer &packet, TcpPrctl::Type type) const noexcept;
	const std::string &getName() const noexcept;
	const std::vector<PlayerConnection *> &getPlayers() const noexcept;
	void join(PlayerConnection &handle) noexcept;
	void leave(PlayerConnection &handle) noexcept;
	void start(const std::function<void(GameRoom &)> &endCallback) noexcept;
	void stop() noexcept;
	bool isRunning() const noexcept;

      protected:
	void scene() noexcept;
	std::string _name;
	std::vector<PlayerConnection *> _players;
	std::unique_ptr<std::thread> _thread;
	std::function<void(GameRoom &)> _endCallback;
	sfs::Scene *_scene;
	bool _isRunning;
};
} // namespace cf