#pragma once

#include <thread>
#include <memory>

#include "PlayerConnection.hpp"

namespace cf
{
class GameRoom
{
      public:
	GameRoom(const std::string &name) noexcept;
	~GameRoom() noexcept;
	void sendAllTcp(Serializer &packet, enum pktType_e type) const noexcept;
	const std::string &getName() const noexcept;
	const std::vector<PlayerConnection *> &getPlayers() const noexcept;
	void join(PlayerConnection &handle) noexcept;
	void leave(PlayerConnection &handle) noexcept;
	void start(const std::function<void(GameRoom &)> &endCallback) noexcept;

      protected:
	void scene() noexcept;
	std::string _name;
	std::vector<PlayerConnection *> _players;
	std::unique_ptr<std::thread> _thread;
	std::function<void(GameRoom &)> _endCallback;
};
} // namespace cf