#include <boost/bind.hpp>

#include "GoUdp.hpp"

namespace cf {
GoUdp::GoUdp(GameManager &manager, uint16_t port) noexcept
	: _udpService()
	, _lastRemote()
	, _commonSocket(_udpService, udp::endpoint(udp::v4(), port))
	, _manager(manager)
{
	autoBind(UdpPrctl::Type::POSITION, &GoUdp::positionHandler);
	autoBind(UdpPrctl::Type::VELOCITY, &GoUdp::velocityHandler);
	autoBind(UdpPrctl::Type::SPRITE, &GoUdp::spriteHandler);
	autoBind(UdpPrctl::Type::SPAWN, &GoUdp::spawnHandler);
	autoBind(UdpPrctl::Type::INPUT, &GoUdp::inputHandler);
	autoBind(UdpPrctl::Type::TIME, &GoUdp::timeHandler);
	autoBind(UdpPrctl::Type::UNKNOWN, &GoUdp::unknownHandler);
	autoBind(UdpPrctl::Type::ACK, &GoUdp::ackHandler);
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
			header.type = static_cast<uint32_t>(UdpPrctl::Type::ACK);
			_commonSocket.send_to(boost::asio::buffer(&header, sizeof(header)),
					      net->getUdpRemote());
		}
		if (net->getUdpHeader().isCorrect()
		    && net->getUdpHeader().getLength() >= net->getUdpSerializer().getSize()) {
			std::cout << "udp packet type: "
				  << static_cast<uint32_t>(net->getUdpHeader().getType())
				  << std::endl;
		}
	}
}

bool GoUdp::executePacket(PlayerConnection &connection) noexcept
{
	if (connection.getUdpHeader().isCorrect()) {
		if (connection.getUdpHeader().getType() == UdpPrctl::Type::ACK) {
			connection.notifyUdpReceive(connection.getUdpHeader().getIndex());
			return true;
		}
		else if (connection.getUdpHeader().getLength()
			 >= connection.getUdpSerializer().getSize()) {
			auto *go = getPlayerFromConnection(connection);
			if (go == nullptr)
				return false;
			return _callbacks[static_cast<int>(connection.getUdpHeader().getType())](
				       *go, connection.getUdpSerializer())
			       == 0;
		}
	}
	return false;
}

void GoUdp::computePackets(PlayerConnection &connection) noexcept
{
	executePacket(connection);
	while (connection.getUdpSerializer().getSize() >= sizeof(UdpPrctl::udpHeader)
	       && executePacket(connection))
		;
}

GoPlayer *GoUdp::getPlayerFromConnection(PlayerConnection &connection) const noexcept
{
	const auto &connections = _manager.getConnections();

	for (size_t i = 0; i < connections.size(); ++i) {
		if (connections[i]->getId() == connection.getId()) {
			return _manager.getGoPlayers()[i];
		}
	}
	return nullptr;
}

void GoUdp::start(sfs::Scene &scene) noexcept
{
	(void)scene;
	asyncReceive();
}

void GoUdp::update(sfs::Scene &scene) noexcept
{
	(void)scene;
	_udpService.poll();
	for (auto &&i : _manager.getConnections())
		i->refreshUdp(_commonSocket);
	std::cout << _manager.getGoPlayers()[0]->asString() << std::endl;
}
} // namespace cf