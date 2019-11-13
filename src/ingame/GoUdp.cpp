#include <boost/bind.hpp>

#include "GoUdp.hpp"

namespace cf {
GoUdp::GoUdp(GameManager &manager, uint16_t port) noexcept
	: _udpService()
	, _lastRemote()
	, _commonSocket(_udpService, udp::endpoint(udp::v4(), port))
	, _manager(manager)
{
}

void GoUdp::asyncReceive() noexcept
{
	_commonSocket.async_receive_from(boost::asio::buffer(_commonBuffer), _lastRemote,
					 boost::bind(&GoUdp::onUpdate, this,
						     boost::asio::placeholders::error,
						     boost::asio::placeholders::bytes_transferred));
}

PlayerConnection *GoUdp::getClient(const udp::endpoint &remote) const noexcept
{
	for (auto &&i : _manager.getConnections()) {
		if (i->getUdpRemote().address() == remote.address())
			return i;
	}
	return nullptr;
}

void GoUdp::onUpdate(const boost::system::error_code &error, std::size_t bytes_transferred)
{
	if (error) {
		std::cerr << error.message() << std::endl;
		return;
	}
	asyncReceive();
	auto net = getClient(_lastRemote);
	if (net) {
		net->getUdpSerializer().nativeSet(_commonBuffer.data(), bytes_transferred);
		std::cout << _lastRemote.address() << " : " << bytes_transferred << "o"
			  << std::endl;
		if (net->getUdpSerializer().getSize() >= sizeof(UdpPrctl::udpHeader)) {
			UdpPrctl::udpHeader header;
			net->getUdpSerializer().get(header);
			net->setUdpHeader(header);
		}
		if (net->getUdpHeader().isCorrect()
		    && net->getUdpHeader().getLength() >= net->getUdpSerializer().getSize()) {
			std::cout << "udp packet type: "
				  << static_cast<uint32_t>(net->getUdpHeader().getType())
				  << std::endl;
		}
	}
}

void GoUdp::start(sfs::Scene &scene) noexcept
{
	(void)scene;
	asyncReceive();
}
static float a = 0;
void GoUdp::update(sfs::Scene &scene) noexcept
{
	_udpService.poll();
	a += scene.deltaTime();
	if (a > 1.f) {
		Serializer s;
		s.set(std::string("hello"));
		a = 0;
		for (auto &&i : _manager.getConnections()) {
			i->pushUdpPacket(s, UdpPrctl::Type::TIME);
		}
	}
	for (auto &&i : _manager.getConnections())
		i->refreshUdp(_commonSocket);
}
} // namespace cf