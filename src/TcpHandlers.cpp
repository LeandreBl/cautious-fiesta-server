#include <filesystem>

#include "Server.hpp"

namespace cf
{
int Server::loginHandler(PlayerConnection &handle, Serializer &toRead)
{
	std::string nickname;
	Player::stats stats;
	Serializer answer;
	uint8_t isOk = true;

	if (!toRead.get(nickname) || !toRead.get(stats) || nickname.empty()) {
		answer.set(false);
		handle.pushPacket(answer, cf::LOGIN);
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
		handle.setPlayer(stats);
	}
	answer.set(isOk);
	handle.pushPacket(answer, cf::LOGIN);
	auto &player = handle.getPlayer();
	say(isOk,
	    "~%s logged in { life: %.0f, speed: %.1f, attack: %.1f, attackSpeed: %.1f, armor: %.1f (%.1f%%) }\n",
	    handle.name().c_str(), player.getLife(), player.getSpeed(),
	    player.getAttack(), player.getAttackSpeed(), player.getArmor(),
	    player.getArmorCoefficient());
	return 0;
} // namespace cf

void Server::fillGameRooms(Serializer &packet) const noexcept
{
	packet.set((uint64_t)_gameRooms.size());
	for (auto &&i : _gameRooms) {
		packet.set((uint64_t)i.getPlayers().size());
		packet.set(i.getName());
	}
}

void Server::fillGameRoomPlayers(const std::string &roomName,
				 Serializer &packet) const noexcept
{
	for (auto &&i : _gameRooms) {
		if (i.getName() == roomName) {
			packet.set(true);
			packet.set(i.getName());
			packet.set((uint64_t)i.getPlayers().size());
			for (auto &&i : i.getPlayers()) {
				packet.set(i->ready());
				packet.set(i->name());
			}
			return;
		}
	}
	packet.set(false);
	packet.set(std::string(""));
	packet.set((uint64_t)0);
}

void Server::resendGameRoomsHandler() noexcept
{
	Serializer answer;

	fillGameRooms(answer);
	for (auto &&i : _connectionPool) {
		if (i.isLogged())
			i.pushPacket(answer, cf::GET_GAMEROOMS_LIST);
	}
}

int Server::logoutHandler(PlayerConnection &handle, Serializer &)
{
	uint8_t ok = handle.isLogged();
	Serializer answer;

	answer.set(ok);
	handle.pushPacket(answer, cf::LOGOUT);
	if (ok == false) {
		say(false, "~%s logged out\n", handle.name().c_str());
		return 0;
	}
	if (handle.isInRoom()) {
		Serializer answer;
		fillGameRoomPlayers(handle.room().getName(), answer);
		handle.room().sendAllTcp(answer, cf::GET_GAMEROOM_PLAYERS_LIST);
		answer.clear();
		fillGameRooms(answer);
		handle.room().sendAllTcp(answer, cf::GET_GAMEROOMS_LIST);
		handle.room().leave(handle);
	}
	handle.refreshTcp();
	say(true, "~%s logged out\n", handle.name().c_str());
	handle.name("");
	return 1;
}

int Server::createGameRoomHandler(PlayerConnection &handle, Serializer &toRead)
{
	Serializer answer;
	std::string name;

	if (!toRead.get(name))
		return -1;
	for (auto &&i : _gameRooms) {
		if (i.getName() == name) {
			answer.set(false);
			handle.pushPacket(answer, cf::CREATE_GAMEROOM);
			say(false, "[%s]: {%s} already exist\n", __FUNCTION__,
			    name.c_str());
			return 0;
		}
	}
	answer.set(true);
	handle.pushPacket(answer, cf::CREATE_GAMEROOM);
	_gameRooms.emplace_back(name);
	resendGameRoomsHandler();
	say(true, "{%s} created\n", name.c_str());
	return 0;
}

void Server::kickRoomPlayers(PlayerConnection &handle) noexcept
{
	Serializer packet;
	auto room = handle.room();

	packet.set(true);
	for (auto &&i : room.getPlayers()) {
		i->pushPacket(packet, cf::LEAVE_GAMEROOM);
		i->leaveRoom();
	}
}

int Server::deleteGameRoomHandler(PlayerConnection &handle, Serializer &)
{
	Serializer answer;

	if (handle.isInRoom() == false) {
		answer.set(false);
		handle.pushPacket(answer, cf::DELETE_GAMEROOM);
		return 0;
	}
	for (auto it = _gameRooms.begin(); it != _gameRooms.end(); ++it) {
		if (it->getName() == handle.room().getName()) {
			answer.set(true);
			handle.pushPacket(answer, cf::DELETE_GAMEROOM);
			say(true, "{%s} deleted\n",
			    handle.room().getName().c_str());
			kickRoomPlayers(handle);
			_gameRooms.erase(it);
			resendGameRoomsHandler();
			return 0;
		}
	}
	say(false, "[%s] {%s} not found\n", __FUNCTION__,
	    handle.room().getName().c_str());
	return -1;
}

int Server::getGameRoomsHandler(PlayerConnection &handle, Serializer &)
{
	Serializer answer;

	fillGameRooms(answer);
	handle.pushPacket(answer, cf::GET_GAMEROOMS_LIST);
	return 0;
}

int Server::joinGameRoomHandler(PlayerConnection &handle, Serializer &toRead)
{
	Serializer answer;
	std::string name;

	if (!toRead.get(name))
		return -1;
	for (auto &&i : _gameRooms) {
		if (i.getName() == name) {
			i.join(handle);
			answer.set(true);
			handle.pushPacket(answer, cf::JOIN_GAMEROOM);
			resendGameRoomsHandler();
			resendPlayerListHandler();
			sendRequiredAssets(handle);
			say(true, "~%s joined {%s}\n", handle.name().c_str(),
			    i.getName().c_str());
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

	answer.set(ok);
	if (ok == true) {
		say(ok, "~%s left {%s}\n", handle.name().c_str(),
		    handle.room().getName().c_str());
		handle.leaveRoom();
	}
	handle.pushPacket(answer, cf::LEAVE_GAMEROOM);
	resendGameRoomsHandler();
	resendPlayerListHandler();
	return 0;
}

int Server::getGameRoomPlayersListHandler(PlayerConnection &handle,
					  Serializer &toRead)
{
	Serializer answer;
	std::string name;

	if (!toRead.get(name))
		return -1;
	fillGameRoomPlayers(name, answer);
	handle.pushPacket(answer, cf::GET_GAMEROOM_PLAYERS_LIST);
	return 0;
}

int Server::sendGameRoomMessageHandler(PlayerConnection &handle,
				       Serializer &toRead)
{
	Serializer answer;
	std::string message;

	if (!toRead.get(message))
		return -1;
	if (handle.isInRoom() == false) {
		answer.set(false);
		handle.pushPacket(answer, cf::SEND_MESSAGE);
		return 0;
	}
	answer.set(true);
	handle.pushPacket(answer, cf::SEND_MESSAGE);
	answer.clear();
	answer.set(handle.name());
	answer.set(message);
	handle.room().sendAllTcp(answer, cf::RECEIVE_MESSAGE);
	say(true, "~%s - \"%s\"\n", handle.name().c_str(), message.c_str());
	return 0;
}

int Server::receiveGameRoomMessageHandler(PlayerConnection &, Serializer &)
{
	say(false, "UNHANDLED: Received a cf::RECEIVE_MESSAGE packet\n");
	return -1;
}

void Server::resendPlayerListHandler() noexcept
{
	for (auto &&i : _connectionPool) {
		if (i.isInRoom()) {
			Serializer answer;
			fillGameRoomPlayers(i.room().getName(), answer);
			i.pushPacket(answer, cf::GET_GAMEROOM_PLAYERS_LIST);
		}
	}
}

int Server::toggleReadyHandler(PlayerConnection &handle, Serializer &)
{
	Serializer answer;

	if (handle.isInRoom() == false)
		handle.ready(false);
	else
		handle.ready(!handle.ready());
	answer.set(handle.ready());
	handle.pushPacket(answer, cf::TOGGLE_READY);
	say(true, "~%s: %s\n", handle.name().c_str(),
	    (handle.ready() ? "ready" : "not ready"));
	resendPlayerListHandler();
	return 0;
}

int Server::requireAssetHandler(PlayerConnection &handle, Serializer &toRead)
{
	uint8_t state;

	if (!toRead.get(state))
		return -1;
	handle.assetReady(state);
	say(state, "~%s: assets ready\n", (state == true) ? "true" : "false");
	return 0;
}

void Server::sendRequiredAssets(PlayerConnection &handle) noexcept
{
	uint64_t n = 0;
	Serializer answer;
	auto folder = std::filesystem::recursive_directory_iterator("assets/");

	for (auto &&f : folder) {
		if (f.is_regular_file()) {
			++n;
			answer.set(f.path().string());
			answer.set(static_cast<uint64_t>(f.file_size()));
			answer.set(static_cast<uint64_t>(
				easyChksum(f.path().string())));
		}
	}
	answer.forceSetFirst(n);
	handle.pushPacket(answer, cf::ASSETS_REQUIREMENT);
}

void Server::assetWriterCallback(AssetHandler &handler,
				 const boost::system::error_code &gerr)
{
	auto rd = handler.file.readsome(handler.buffer, sizeof(handler.buffer));
	auto buffer = boost::asio::buffer(handler.buffer, rd);

	if (!gerr && rd > 0) {
		handler.receiver.async_write_some(
			buffer,
			std::bind(&Server::assetWriterCallback, this,
				  std::ref(handler), std::placeholders::_1));
	} else {
		boost::system::error_code err;
		handler.receiver.write_some(buffer, err);
		say(!err, "$%u asset sent \"%s\".\n", handler.port,
		    handler.filename.c_str());
		handler.file.close();
		handler.receiver.close();
		for (auto it = _assetsHandlers.begin();
		     it != _assetsHandlers.end(); ++it) {
			if (it->port == handler.port) {
				_assetsHandlers.erase(it);
				return;
			}
		}
	}
}

void Server::assetListenerCallback(AssetHandler &handler)
{
	say(true, "$%u sending asset \"%s\".\n", handler.port,
	    handler.filename.c_str());
	handler.acceptor.close();
	handler.file.open(handler.filename);
	assetWriterCallback(handler, boost::system::error_code());
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

	if (!toRead.get(filename))
		return -1;
	if (std::filesystem::is_regular_file(filename) == false) {
		say(false, "~%s: asset \"%s\" does not exist.\n",
		    handle.name().c_str(), filename.c_str());
		return 0;
	} else if (isValidPath(filename) == false) {
		say(false, "~%s: asset \"%s\" is not a valid path.\n",
		    handle.name().c_str(), filename.c_str());
		return 0;
	}
	auto &l = _assetsHandlers.emplace_back(*this, filename);
	answer.set(l.port);
	answer.set(l.filesize);
	answer.set(l.filename);
	answer.set(l.chksum);
	handle.pushPacket(answer, cf::ASSETS_SEND);
	say(true, "~%s: asset \"%s\" on port %u.\n", handle.name().c_str(),
	    l.filename.c_str(), l.port);
	l.acceptor.async_accept(
		l.receiver,
		std::bind(&Server::assetListenerCallback, this, std::ref(l)));
	return 0;
}

int Server::gameStartHandler(PlayerConnection &handle, Serializer &toRead)
{
	(void)handle;
	(void)toRead;
	return 0;
}

int Server::ackHandler(PlayerConnection &handle, Serializer &)
{
	Serializer answer;

	handle.pushPacket(answer, cf::ACK);
	say(true, "~%s #ACK#\n", handle.name().c_str());
	return 0;
}

int Server::packetHandler(PlayerConnection &handle,
			  const TcpPacketHeader &packetHeader,
			  Serializer &payload) noexcept
{
	packetHeader.display();
	if (packetHeader.isCorrect()
	    && _callbacks[packetHeader.getType()](handle, payload) >= 0) {
		refreshTcpConnections();
		return 0;
	}
	return -1;
}
} // namespace cf