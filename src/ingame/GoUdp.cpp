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

void GoUdp::testAndEmplace(PlayerConnection &net) noexcept
{
	if (net.getUdpSerializer().getSize() >= sizeof(UdpPrctl::udpHeader)) {
		UdpPrctl::udpHeader header;
		net.getUdpSerializer().get(header);
		UdpPrctl p(header);
		if (!p.isCorrect())
			net.getUdpSerializer().clear();
		else if (p.getType() == UdpPrctl::Type::ACK) {
			net.notifyUdpReceive(p.getIndex());
		}
		else if (net.getUdpSerializer().getSize() >= p.getLength()) {
			auto *go = getPlayerFromConnection(net);
			if (go != nullptr)
				_toProcess.emplace(p, net, *go);
		}
		else {
			net.getUdpSerializer().forceSetFirst(p.getNativeHandle());
		}
	}
}

void GoUdp::testAndEmplace(PlayerConnection &net, GoPlayer &player) noexcept
{
	if (net.getUdpSerializer().getSize() >= sizeof(UdpPrctl::udpHeader)) {
		UdpPrctl::udpHeader header;
		net.getUdpSerializer().get(header);
		UdpPrctl p(header);
		if (!p.isCorrect())
			net.getUdpSerializer().clear();
		else if (p.getType() == UdpPrctl::Type::ACK) {
			net.notifyUdpReceive(p.getIndex());
		}
		else if (net.getUdpSerializer().getSize() >= p.getLength()) {
			_toProcess.emplace(p, net, player);
			p.getNativeHandle().type = static_cast<int>(UdpPrctl::Type::ACK);
			net.pushUdpAck(p);
		}
		else {
			net.getUdpSerializer().forceSetFirst(p.getNativeHandle());
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
	testAndEmplace(connection);
	executePackets();
}

void GoUdp::executePackets() noexcept
{

	while (!_toProcess.empty()) {
		auto &p = _toProcess.front();
		auto &header = std::get<0>(p);
		auto &net = std::get<1>(p);
		auto &go = std::get<2>(p);

		_callbacks[static_cast<int>(header.getType())](go, net.getUdpSerializer());
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
		s.set(port);
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