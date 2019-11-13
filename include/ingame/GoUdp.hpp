#pragma once

#include <boost/asio.hpp>
#include <GameObject.hpp>

#include "GameManager.hpp"
#include "PlayerConnection.hpp"

namespace cf {

using boost::asio::ip::udp;

class GoUdp : public sfs::GameObject {
      public:
	GoUdp(GameManager &manager, uint16_t port) noexcept;
	void start(sfs::Scene &scene) noexcept;
	void update(sfs::Scene &scene) noexcept;

      private:
	PlayerConnection *getClient(const udp::endpoint &remote) const noexcept;
	void onUpdate(const boost::system::error_code &error, std::size_t bytes_transferred);
	void computePackets(PlayerConnection &connection) noexcept;
	bool executePacket(PlayerConnection &connection) noexcept;
	void asyncReceive() noexcept;
	boost::asio::io_service _udpService;
	udp::endpoint _lastRemote;
	std::array<uint8_t, 1 << 12> _commonBuffer;
	udp::socket _commonSocket;
	GameManager &_manager;
};
} // namespace cf