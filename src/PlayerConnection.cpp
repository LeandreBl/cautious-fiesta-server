#include "PlayerConnection.hpp"

namespace cf
{
PlayerConnection::PlayerConnection(std::unique_ptr<tcp::socket> &socket) noexcept
: _tcpSocket(std::move(socket))
{

}
} // namespace cf
