#include <boost/bind.hpp>

#include <Trace.hpp>

#include "GoUdp.hpp"

namespace cf {
GoUdp::GoUdp(sfs::Scene &scene, GameManager &manager, boost::asio::io_service &service) noexcept
	: _scene(scene)
	, _ingameService(service)
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
	connection.getIngameSocket().async_receive(
		boost::asio::buffer(connection.getUdpBuffer().buffer),
		boost::bind(&GoUdp::onUpdate, this, std::ref(connection),
			    boost::asio::placeholders::error,
			    boost::asio::placeholders::bytes_transferred));
}

void GoUdp::onUpdate(PlayerConnection &connection, const boost::system::error_code &error,
		     std::size_t bytes_transferred)
{
	if (error) {
		std::cerr << error.message() << std::endl;
		return;
	}
	asyncReceive(connection);
	if (bytes_transferred < sizeof(UdpPrctl))
		return;
	connection.getUdpBuffer().serializer.nativeSet(connection.getUdpBuffer().buffer.data(),
						       bytes_transferred);
}

void GoUdp::executePackets(PlayerConnection &connection) noexcept
{
	auto &s = connection.getUdpBuffer().serializer;

	while (s.getSize() > sizeof(UdpPrctl)) {
		UdpPrctl header;
		s >>= header;
		if (header.isCorrect() == false) {
			s.clear();
			return;
		}
		if (s.getSize() < header.getLength() + sizeof(header))
			return;
		s.shift(sizeof(header));
		auto *go = getPlayerFromConnection(connection);
		size_t idx = static_cast<int>(header.getType());
		if (go != nullptr && _callbacks[idx])
			_callbacks[idx](*go, s);
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
	tcp::acceptor listener(_ingameService, tcp::endpoint(tcp::v4(), 0));
	auto &connections = _manager.getConnections();
	uint16_t port = listener.local_endpoint().port();
	for (auto &&i : connections) {
		Serializer s;
		s << port;
		i->pushPacket(s, TcpPrctl::Type::GAME_STARTED);
		i->refreshTcp();
		tcp::socket &socket = i->setSocket(_ingameService);
		boost::system::error_code e;
		listener.accept(socket, e);
		if (e) {
			trace(false, "~%s failed to join #%s (stopping ...)\n", i->name().c_str(),
			      _manager.getName().c_str());
			scene.clear();
			scene.close();
			return;
		}
		trace(true, "~%s joined ingame #%s\n", i->name().c_str(),
		      _manager.getName().c_str());
	}
	trace(true, "#%s started with %zu player(s).\n", _manager.getName().c_str(),
	      connections.size());
	for (auto &&i : connections)
		asyncReceive(*i);
}

void GoUdp::pollPackets() noexcept
{
	for (auto &&i : _manager.getConnections()) {
		executePackets(*i);
	}
}

void GoUdp::update(sfs::Scene &scene) noexcept
{
	(void)scene;
	_ingameService.poll();
	pollPackets();
	for (auto &&i : _manager.getConnections())
		i->refreshUdp();
}
} // namespace cf