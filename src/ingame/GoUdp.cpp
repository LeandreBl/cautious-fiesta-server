#include <boost/bind.hpp>

#include "GoUdp.hpp"

namespace cf {
GoUdp::GoUdp(GameManager &manager) noexcept
	: _udpService()
	, _lastRemote()
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
		if (net->getUdpSerializer().getSize() >= sizeof(UdpPrctl::udpHeader)) {
			UdpPrctl::udpHeader header;
			net->getUdpSerializer().get(header);
			UdpPrctl p(header);
			if (!p.isCorrect())
				net->getUdpSerializer().clear();
			else if (net->getUdpSerializer().getSize() >= p.getLength()) {
				_toProcess.emplace(p, net);
			}
			else {
				net->getUdpSerializer().forceSetFirst(p.getNativeHandle());
			}
		}
		executePackets();
	}
}

void GoUdp::executePackets() noexcept
{
	while (!_toProcess.empty()) {
		auto &p = _toProcess.front();
		auto *go = getPlayerFromConnection(*p.second);
		if (go == nullptr) {
			_toProcess.pop();
			continue;
		}
		else {
			_callbacks[static_cast<int>(p.first.getType())](
				*go, p.second->getUdpSerializer());
		}
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
	}
	asyncReceive();
}

void GoUdp::update(sfs::Scene &scene) noexcept
{
	(void)scene;
	_udpService.poll();
	for (auto &&i : _manager.getConnections())
		i->refreshUdp(_commonSocket);
}
} // namespace cf