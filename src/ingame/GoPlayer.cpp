#include "GoPlayer.hpp"

namespace cf {
GoPlayer::GoPlayer(const std::string &playerName, const Player &player) noexcept
	: Player(player)
	, _name(playerName)
{
}
} // namespace cf