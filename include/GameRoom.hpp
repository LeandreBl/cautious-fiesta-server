#pragma once

#include "PlayerConnection.hpp"

namespace cf
{
class GameRoom
{
      public:
	GameRoom(const std::string &name) noexcept;
	void sendAllTcp(Serializer &packet, enum pktType_e type) const noexcept;
	const std::string &getName() const noexcept;
	const std::vector<PlayerConnection *> &getPlayers() const noexcept;
	void join(PlayerConnection &handle) noexcept;
	void leave(PlayerConnection &handle) noexcept;

      protected:
	std::string _name;
	std::vector<PlayerConnection *> _players;
	// thread
};
} // namespace cf