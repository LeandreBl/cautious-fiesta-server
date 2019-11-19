#include <filesystem>
#include <boost/bind.hpp>

#include "Server.hpp"

namespace cf {
int Server::loginHandler(PlayerConnection &handle, Serializer &toRead)
{
	std::string nickname;
	Stats::stats stats;
	sf::Color color;
	Serializer answer;
	uint8_t isOk = true;

	if (!toRead.get(nickname) || !toRead.get(stats) || !toRead.get(color) || nickname.empty()) {
		answer.set(false);
		handle.pushPacket(answer, TcpPrctl::Type::LOGIN);
		return -1;
	}
	for (auto &&i : _connectionPool) {
		if (i.name() == nickname) {
			isOk = false;
			break;
		}
	}
	if (isOk == true) {
		handle.name(nickname);
		handle.setPlayer(stats, color);
	}
	answer.set(isOk);
	handle.pushPacket(answer, TcpPrctl::Type::LOGIN);
	auto &player = handle.getPlayer();
	say(isOk,
	    "~%s logged in { life: %.0f, speed: %.1f, attack: %.1f, attackSpeed: %.1f, armor: %.1f "
	    "(%.1f%%), color: (r: %d, g: %d, b: %d) }\n",
	    handle.name().c_str(), player.getLife(), player.getSpeed(), player.getAttack(),
	    player.getAttackSpeed(), player.getArmor(), player.getArmorCoefficient(),
	    player.getColor().r, player.getColor().g, player.getColor().b);
	return 0;
} // namespace cf

int Server::logoutHandler(PlayerConnection &handle, Serializer &)
{
	uint8_t ok = handle.isLogged();
	Serializer answer;

	answer.set(ok);
	handle.pushPacket(answer, TcpPrctl::Type::LOGOUT);
	if (ok == false) {
		say(false, "~%s logged out\n", handle.name().c_str());
		return 0;
	}
	handle.name("");
	handle.ready(false);
	if (handle.isInRoom()) {
		Serializer answer;
		auto &room = handle.room();
		room.leave(handle);
		fillGameRoomPlayers(room.getName(), answer);
		room.sendAllTcp(answer, TcpPrctl::Type::GET_GAMEROOM_PLAYERS_LIST);
		answer.clear();
		fillGameRooms(answer);
		for (auto &&i : _connectionPool) {
			if (!i.isInRoom() || !i.room().isRunning()) {
				i.pushPacket(answer, TcpPrctl::Type::GET_GAMEROOMS_LIST);
				i.refreshTcp();
			}
		}
	}
	handle.refreshTcp();
	say(true, "~%s logged out\n", handle.name().c_str());
	return 1;
}

int Server::createGameRoomHandler(PlayerConnection &handle, Serializer &toRead)
{
	Serializer answer;
	std::string name;

	if (handle.isLogged() == false)
		return 0;
	if (!toRead.get(name))
		return -1;
	for (auto &&i : _gameRooms) {
		if (i.getName() == name) {
			answer.set(false);
			handle.pushPacket(answer, TcpPrctl::Type::CREATE_GAMEROOM);
			say(false, "[%s]: {%s} already exist\n", __FUNCTION__, name.c_str());
			return 0;
		}
	}
	answer.set(true);
	handle.pushPacket(answer, TcpPrctl::Type::CREATE_GAMEROOM);
	_gameRooms.emplace_back(name);
	resendGameRoomsHandler();
	say(true, "{%s} created\n", name.c_str());
	return 0;
}

int Server::deleteGameRoomHandler(PlayerConnection &handle, Serializer &)
{
	Serializer answer;

	if (handle.isLogged() == false)
		return 0;
	if (handle.isInRoom() == false) {
		answer.set(false);
		handle.pushPacket(answer, TcpPrctl::Type::DELETE_GAMEROOM);
		return 0;
	}
	if (deleteGameRoom(handle.room().getName()) == 0) {
		answer.set(true);
		handle.pushPacket(answer, TcpPrctl::Type::DELETE_GAMEROOM);
		resendGameRoomsHandler();
		return 0;
	}
	say(false, "[%s] {%s} not found\n", __FUNCTION__, handle.room().getName().c_str());
	return -1;
}

int Server::getGameRoomsHandler(PlayerConnection &handle, Serializer &)
{
	Serializer answer;

	if (handle.isLogged() == false)
		return 0;
	fillGameRooms(answer);
	handle.pushPacket(answer, TcpPrctl::Type::GET_GAMEROOMS_LIST);
	return 0;
}

int Server::joinGameRoomHandler(PlayerConnection &handle, Serializer &toRead)
{
	Serializer answer;
	std::string name;

	if (handle.isLogged() == false)
		return 0;
	if (!toRead.get(name))
		return -1;
	for (auto &&i : _gameRooms) {
		if (i.getName() == name) {
			i.join(handle);
			answer.set(true);
			handle.pushPacket(answer, TcpPrctl::Type::JOIN_GAMEROOM);
			resendGameRoomsHandler();
			resendPlayerListHandler();
			sendRequiredAssets(handle);
			say(true, "~%s joined {%s}\n", handle.name().c_str(), i.getName().c_str());
			return 0;
		}
	}
	say(false, "[%s]: {%s} does not exist\n", __FUNCTION__, name.c_str());
	return 0;
}

int Server::leaveGameRoomHandler(PlayerConnection &handle, Serializer &)
{
	Serializer answer;
	uint8_t ok = handle.isInRoom();

	if (handle.isLogged() == false)
		return 0;
	answer.set(ok);
	if (ok == true) {
		say(ok, "~%s left {%s}\n", handle.name().c_str(), handle.room().getName().c_str());
		handle.leaveRoom();
	}
	handle.pushPacket(answer, TcpPrctl::Type::LEAVE_GAMEROOM);
	resendGameRoomsHandler();
	resendPlayerListHandler();
	return 0;
}

int Server::getGameRoomPlayersListHandler(PlayerConnection &handle, Serializer &toRead)
{
	Serializer answer;
	std::string name;

	if (handle.isLogged() == false)
		return 0;
	if (!toRead.get(name))
		return -1;
	fillGameRoomPlayers(name, answer);
	handle.pushPacket(answer, TcpPrctl::Type::GET_GAMEROOM_PLAYERS_LIST);
	return 0;
}

int Server::sendGameRoomMessageHandler(PlayerConnection &handle, Serializer &toRead)
{
	Serializer answer;
	std::string message;

	if (handle.isLogged() == false)
		return 0;
	if (!toRead.get(message))
		return -1;
	if (handle.isInRoom() == false) {
		answer.set(false);
		handle.pushPacket(answer, TcpPrctl::Type::SEND_MESSAGE);
		return 0;
	}
	answer.set(true);
	handle.pushPacket(answer, TcpPrctl::Type::SEND_MESSAGE);
	answer.clear();
	answer.set(handle.name());
	answer.set(message);
	handle.room().sendAllTcp(answer, TcpPrctl::Type::RECEIVE_MESSAGE);
	say(true, "~%s - \"%s\"\n", handle.name().c_str(), message.c_str());
	return 0;
}

int Server::receiveGameRoomMessageHandler(PlayerConnection &, Serializer &)
{
	say(false, "UNHANDLED: Received a TcpPrctl::Type::RECEIVE_MESSAGE packet\n");
	return -1;
}

void Server::resendPlayerListHandler() noexcept
{
	for (auto &&i : _connectionPool) {
		if (i.isInRoom() && !i.room().isRunning()) {
			Serializer answer;
			fillGameRoomPlayers(i.room().getName(), answer);
			i.pushPacket(answer, TcpPrctl::Type::GET_GAMEROOM_PLAYERS_LIST);
		}
	}
}

static bool isStarting(const std::vector<PlayerConnection *> &v) noexcept
{
	for (auto &&i : v)
		if (i->ready() == false)
			return false;
	return true;
}

int Server::toggleReadyHandler(PlayerConnection &handle, Serializer &)
{
	Serializer answer;
	bool start = false;

	if (handle.isLogged() == false)
		return 0;
	else if (handle.isInRoom() == false) {
		handle.ready(false);
	}
	else {
		handle.ready(!handle.ready());
		start = isStarting(handle.room().getPlayers());
	}
	answer.set(handle.ready());
	handle.pushPacket(answer, TcpPrctl::Type::TOGGLE_READY);
	say(true, "~%s: %s\n", handle.name().c_str(), (handle.ready() ? "ready" : "not ready"));
	resendPlayerListHandler();
	if (start == true) {
		startGameRoom(handle.room());
	}
	return 0;
}

int Server::requireAssetHandler(PlayerConnection &handle, Serializer &toRead)
{
	uint8_t state;

	if (handle.isLogged() == false)
		return 0;
	if (!toRead.get(state))
		return -1;
	handle.assetReady(state);
	say(state, "~%s: assets ready: %s\n", handle.name().c_str(),
	    (state == true) ? "true" : "false");
	return 0;
}

static bool isValidPath(const std::string &filename)
{
	auto realpath = std::filesystem::absolute(filename).string();
	auto currentPath = std::filesystem::current_path().string();

	currentPath += "/assets/";
	return realpath.compare(0, currentPath.length(), currentPath) == 0;
}

int Server::sendAssetHandler(PlayerConnection &handle, Serializer &toRead)
{
	std::string filename;
	Serializer answer;

	if (handle.isLogged() == false)
		return 0;
	if (!toRead.get(filename))
		return -1;
	if (std::filesystem::is_regular_file(filename) == false) {
		say(false, "~%s: asset \"%s\" does not exist.\n", handle.name().c_str(),
		    filename.c_str());
		return 0;
	}
	else if (isValidPath(filename) == false) {
		say(false, "~%s: asset \"%s\" is not a valid path.\n", handle.name().c_str(),
		    filename.c_str());
		return 0;
	}
	auto &l = _assetsHandlers.emplace_back(*this, filename);
	answer.set(l.port);
	answer.set(l.filesize);
	answer.set(l.filename);
	answer.set(l.chksum);
	handle.pushPacket(answer, TcpPrctl::Type::ASSETS_SEND);
	say(true, "~%s: asset \"%s\" on port %u.\n", handle.name().c_str(), l.filename.c_str(),
	    l.port);
	l.acceptor.async_accept(l.receiver,
				boost::bind(&Server::assetListenerCallback, this, std::ref(l),
					    boost::asio::placeholders::error));
	return 0;
}

int Server::gameStartHandler(PlayerConnection &handle, Serializer &toRead)
{
	uint16_t port;

	if (!toRead.get(port))
		return -1;
	handle.getUdpRemote().port(port);
	return 0;
}

int Server::ackHandler(PlayerConnection &handle, Serializer &)
{
	Serializer answer;

	handle.pushPacket(answer, TcpPrctl::Type::ACK);
	say(true, "~%s #ACK#\n", handle.name().c_str());
	return 0;
}

int Server::packetHandler(PlayerConnection &handle, const TcpPrctl &packetHeader,
			  Serializer &payload) noexcept
{
	packetHeader.display();
	if (packetHeader.isCorrect() && _callbacks[packetHeader.getType()](handle, payload) >= 0) {
		refreshTcpConnections();
		return 0;
	}
	return -1;
}
} // namespace cf