#include <filesystem>
#include <boost/bind.hpp>

#include <Asset.hpp>

#include "Server.hpp"

namespace cf
{

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
		if (i.isLogged() && (!i.isInRoom() || !i.room().isRunning()))
			i.pushPacket(answer, TcpPrctl::Type::GET_GAMEROOMS_LIST);
	}
}

void Server::kickRoomPlayers(PlayerConnection &handle) noexcept
{
	Serializer packet;
	auto &room = handle.room();

	packet.set(true);
	for (auto &&i : room.getPlayers()) {
		i->pushPacket(packet, TcpPrctl::Type::LEAVE_GAMEROOM);
		i->leaveRoom();
	}
}

int Server::deleteGameRoom(const std::string &name) noexcept
{
	for (auto it = _gameRooms.begin(); it != _gameRooms.end(); ++it)
		if (it->getName() == name) {
			say(true, "{%s} deleted\n", name.c_str());
			_gameRooms.erase(it);
			return 0;
		}
	for (auto it = _runningGameRooms.begin(); it != _runningGameRooms.end();
	     ++it)
		if (it->getName() == name) {
			say(true, "{%s} deleted\n", name.c_str());
			_runningGameRooms.erase(it);
			return 0;
		}
	return -1;
}

void Server::startGameRoom(const GameRoom &room) noexcept
{
	for (auto it = _gameRooms.begin(); it != _gameRooms.end(); ++it) {
		if (it->getName() == room.getName()) {
			_runningGameRooms.splice(_runningGameRooms.end(),
						 _gameRooms, it);
			_runningGameRooms.back().start(
				std::bind(&Server::gameRoomTermination, this,
					  std::placeholders::_1),
				2225);
			return;
		}
	}
}

void Server::gameRoomTermination(GameRoom &room)
{
	for (auto it = _runningGameRooms.begin(); it != _runningGameRooms.end();
	     ++it) {
		if (it->getName() == room.getName()) {
			_runningGameRooms.erase(it);
			return;
		}
	}
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
				common::computeChksum(f.path().string())));
		}
	}
	answer.forceSetFirst(n);
	handle.pushPacket(answer, TcpPrctl::Type::ASSETS_REQUIREMENT);
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

void Server::assetListenerCallback(AssetHandler &handler,
				   const boost::system::error_code &err)
{
	say(!err, "$%u sending asset \"%s\" %s.\n", handler.port,
	    handler.filename.c_str(), err.message().c_str());
	handler.acceptor.close();
	if (err) {
		return;
	}
	handler.file.open(handler.filename);
	assetWriterCallback(handler, boost::system::error_code());
}

} // namespace cf