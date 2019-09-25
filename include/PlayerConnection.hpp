#pragma once

#include <boost/asio.hpp>
#include <memory>

namespace cf
{
using boost::asio::ip::tcp;
using boost::asio::ip::udp;
class PlayerConnection
{
      public:
	PlayerConnection(std::unique_ptr<tcp::socket> &socket) noexcept;
	~PlayerConnection() = default;

      protected:
	std::string _name;
	std::unique_ptr<udp::socket> _udpSocket;
	std::unique_ptr<tcp::socket> _tcpSocket;
};
} // namespace cf