#include <boost/bind.hpp>

#include "GoUdp.hpp"

namespace cf {
GoUdp::GoUdp(GameManager &manager) noexcept
	: _udpService()
	, _commonSocket(_udpService, udp::endpoint(udp::v4(), 0))
	, _manager(manager)
{
	autoBind(UdpPrctl::Type::POSITION, &GoUdp::positionHandler);
	autoBind(UdpPrctl::Type::VELOCITY, &GoUdp::velocityHandler);
	autoBind(UdpPrctl::Type::SPRITE, &GoUdp::spriteHandler);
	autoBind(UdpPrctl::Type::SPAWN, &GoUdp::spawnHandler);
	autoBind(UdpPrctl::Type::INPUT, &GoUdp::inputHandler);
	autoBind(UdpPrctl::Type::TIME, &GoUdp::timeHandler);
	autoBind(UdpPrctl::Type::STATE, &GoUdp::stateHandler);
	autoBind(UdpPrctl::Type::DESTROY, &GoUdp::destroyHandler);
	autoBind(UdpPrctl::Type::ATTACK, &GoUdp::attackHandler);
	autoBind(UdpPrctl::Type::UNKNOWN, &GoUdp::unknownHandler);
	autoBind(UdpPrctl::Type::ACK, &GoUdp::ackHandler);
}

void GoUdp::asyncReceive(PlayerConnection &connection) noexcept
{
	_commonSocket.async_receive_from(boost::asio::buffer(_commonBuffer),
					 connection.getUdpRemote(),
					 boost::bind(&GoUdp::onUpdate, this, std::ref(connection),
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

void GoUdp::testAndEmplace(PlayerConnection &net, GoPlayer &player) noexcept
{
	if (net.getUdpSerializer().getSize() >= sizeof(UdpPrctl::udpHeader)) {
		UdpPrctl header;
		net.getUdpSerializer() >>= header;
		if (!header.isCorrect())
			net.getUdpSerializer().clear();
		else if (header.getType() == UdpPrctl::Type::ACK) {
			net.getUdpSerializer().shift(sizeof(UdpPrctl::udpHeader));
			net.notifyUdpReceive(header.getIndex());
		}
		else if (net.getUdpSerializer().getSize()
			 >= (header.getLength() + sizeof(UdpPrctl::udpHeader))) {
			net.getUdpSerializer().shift(sizeof(UdpPrctl::udpHeader));
			_toProcess.emplace(header,
					   Serializer(net.getUdpSerializer(), header.getLength()),
					   net, player);
			header.getNativeHandle().type = static_cast<int>(UdpPrctl::Type::ACK);
			net.sendUdpAck(_commonSocket, header);
		}
	}
}

void GoUdp::onUpdate(PlayerConnection &connection, const boost::system::error_code &error,
		     std::size_t bytes_transferred)
{
	if (error) {
		std::cerr << error.message() << std::endl;
		return;
	}
	asyncReceive(connection);
	connection.getUdpSerializer().nativeSet(_commonBuffer.data(), bytes_transferred);
	auto *go = getPlayerFromConnection(connection);
	if (go != nullptr)
		testAndEmplace(connection, *go);
	executePackets();
}

void GoUdp::executePackets() noexcept
{
	while (!_toProcess.empty()) {
		auto &p = _toProcess.front();
		auto &header = std::get<0>(p);
		auto &serializer = std::get<1>(p);
		auto &net = std::get<2>(p);
		auto &go = std::get<3>(p);
		_callbacks[static_cast<int>(header.getType())](go, serializer);
		testAndEmplace(net, go);
		_toProcess.pop();
	}
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
	auto &connections = _manager.getConnections();
	uint16_t port = _commonSocket.local_endpoint().port();
	for (auto &&i : connections) {
		Serializer s;
		s << port;
		i->pushPacket(s, TcpPrctl::Type::GAME_STARTED);
		i->refreshTcp();
		asyncReceive(*i);
	}
}

void GoUdp::update(sfs::Scene &scene) noexcept
{
	(void)scene;
	_udpService.poll();
	for (auto &&i : _manager.getConnections())
		i->refreshUdp(_commonSocket);
}
} // namespace cf